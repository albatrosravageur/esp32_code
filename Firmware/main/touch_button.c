/* Touch Pad Interrupt Example

   This example code is in the Public Domain (or CC0 licensed, at your option.)

   Unless required by applicable law or agreed to in writing, this
   software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
   CONDITIONS OF ANY KIND, either express or implied.
*/
#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include "esp_log.h"
#include "driver/touch_pad.h"
#include "soc/rtc_periph.h"
#include "soc/sens_periph.h"
#include "touch_button.h"

static const char *TAG = "Touch pad";

#define TOUCH_THRESH_NO_USE   (0)
#define TOUCH_THRESH_PERCENT  (80)
#define TOUCHPAD_FILTER_TOUCH_PERIOD (10)
#define TOUCHPAD_ID (0)

static bool s_pad_activated;
static uint32_t s_pad_init_val;

/*
  Read values sensed at all available touch pads.
  Use 2 / 3 of read value as the threshold
  to trigger interrupt when the pad is touched.
  Note: this routine demonstrates a simple way
  to configure activation threshold for the touch pads.
  Do not touch any pads when this routine
  is running (on application start).
 */
static void tp_example_set_thresholds(void)
{
    uint16_t touch_value;

    touch_pad_read_filtered(TOUCHPAD_ID, &touch_value);

    s_pad_init_val = touch_value;
    ESP_LOGI(TAG, "test init: touch pad val is %d and I changed that line", touch_value);
    //set interrupt threshold.
    ESP_ERROR_CHECK(touch_pad_set_thresh(TOUCHPAD_ID, touch_value * 2 / 3));
}

/*
  Check if any of touch pads has been activated
  by reading a table updated by rtc_intr()
  If so, then print it out on a serial monitor.
  Clear related entry in the table afterwards

  In interrupt mode, the table is updated in touch ISR.

  In filter mode, we will compare the current filtered value with the initial one.
  If the current filtered value is less than 80% of the initial value, we can
  regard it as a 'touched' event.
  When calling touch_pad_init, a timer will be started to run the filter.
  This mode is designed for the situation that the pad is covered
  by a 2-or-3-mm-thick medium, usually glass or plastic.
  The difference caused by a 'touch' action could be very small, but we can still use
  filter mode to detect a 'touch' event.
 */
int activated =0;
int getactivated(){
    return activated;
}
static void tp_example_read_task(void *pvParameter)
{
    int filter_mode = 0;
    while (1) {
        if (filter_mode == 0) {
            //interrupt mode, enable touch interrupt
            touch_pad_intr_enable();
            if (s_pad_activated == true) {
                ESP_LOGI(TAG, "TouchPad activated!");
                // Wait a while for the pad being released
                vTaskDelay(200 / portTICK_PERIOD_MS);
                // Clear information on pad activation
                s_pad_activated = false;
            }
            else {
                //filter mode, disable touch interrupt
                touch_pad_intr_disable();
                touch_pad_clear_status();
                    uint16_t value = 0;
                    touch_pad_read_filtered(TOUCHPAD_ID, &value);
                    if (value < s_pad_init_val * TOUCH_THRESH_PERCENT / 100) {
                        activated=1;
                        ESP_LOGI(TAG, "TouchPad activated!");
                        ESP_LOGI(TAG, "value: %d; init val: %d", value, s_pad_init_val);
                        vTaskDelay(200 / portTICK_PERIOD_MS);
                        // Reset the counter to stop changing mode.
                    }
                }
            }

        vTaskDelay(10 / portTICK_PERIOD_MS);
    }
}

/*
  Handle an interrupt triggered when a pad is touched.
  Recognize what pad has been touched and save it in a table.
 */
static void tp_example_rtc_intr(void *arg)
{
    uint32_t pad_intr = touch_pad_get_status();
    //clear interrupt
    touch_pad_clear_status();
    if ((pad_intr) & 0x01) {
        s_pad_activated = true;
    }
}

/*
 * Before reading touch pad, we need to initialize the RTC IO.
 */
static void tp_example_touch_pad_init(void)
{
    //init RTC IO and mode for touch pad.
    touch_pad_config(TOUCHPAD_ID, TOUCH_THRESH_NO_USE);
}

void touch_main(void)
{
    // Initialize touch pad peripheral, it will start a timer to run a filter
    ESP_LOGI(TAG, "Initializing touch pad");
    touch_pad_init();
    // If use interrupt trigger mode, should set touch sensor FSM mode at 'TOUCH_FSM_MODE_TIMER'.
    touch_pad_set_fsm_mode(TOUCH_FSM_MODE_TIMER);
    // Set reference voltage for charging/discharging
    // For most usage scenarios, we recommend using the following combination:
    // the high reference valtage will be 2.7V - 1V = 1.7V, The low reference voltage will be 0.5V.
    touch_pad_set_voltage(TOUCH_HVOLT_2V7, TOUCH_LVOLT_0V5, TOUCH_HVOLT_ATTEN_1V);
    // Init touch pad IO
    tp_example_touch_pad_init();
    // Initialize and start a software filter to detect slight change of capacitance.
    touch_pad_filter_start(TOUCHPAD_FILTER_TOUCH_PERIOD);
    // Set thresh hold
    tp_example_set_thresholds();
    // Register touch interrupt ISR
    touch_pad_isr_register(tp_example_rtc_intr, NULL);
    // Start a task to show what pads have been touched
    xTaskCreate(&tp_example_read_task, "touch_pad_read_task", 2048, NULL, 5, NULL);
}