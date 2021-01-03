#include <Arduino.h>
#define Utilitaires_h

// Leds APA102
#define LED_DATA_PIN 18
#define LED_CLOCK_PIN 5
#define BRIGHTNESS 64
#define NUM_LEDS 60
#define LED_TYPE APA102
#define COLOR_ORDER GRB
#define UPDATES_PER_SECOND 2
#define LED_BLINK_DELAY 250 //ms

// Touch
#define TOUCH_PIN TOUCH_PAD_NUM7_GPIO_NUM
#define TOUCH_NUM TOUCH_PAD_NUM7

#define TOUCH_REFRESH_PERIOD 50
#define TOUCHPAD_FILTER_TOUCH_PERIOD (50)
#define TIME_PP_MS 1000
#define TIME_NEXT_MS 2000
#define TIME_OFF_MS 3000

// Firebase
#define POINT_MAX_TIME 100

// Wifi
#define MAX_WIFI_RESEARCH_TIME 60 //s

// Leds for battery
#define LED_CHARGED_PIN 15
#define LED_CHARGE_PIN 32
#define BAT_LEVEL_READER A12

// FREE-RTOS
// Core repartition of threads
#define BAT_CORE 0
#define TOUCH_CORE 0
#define BT_CORE 0
#define FIRE_CORE 0
#define WIFI_CORE 0

// Priorities among threads (1 is min, 5 is max)
#define BAT_PRIORITY 1
#define TOUCH_PRIORITY 3
#define BT_PRIORITY 2
#define FIRE_PRIORITY 2
#define WIFI_PRIORITY 2

// Bluetooth
#define DEVICE_NAME "Odeji-123456" // This is a unique ID

enum bt_messages_received
{
    SET_WIFI_SSID = '0',
    GET_WIFI_SSID = '1',
    SET_WIFI_PASSWORD = '2',
    GET_WIFI_PASSWORD = '3',
    SET_MEETING_ID = '4',
    GET_MEETING_ID = '5',
    GO_CONNECT_FIREBASE = '6',
    DISCONNECT_FIREBASE = '7',
    GO_CONNECT_WIFI = '8',
    STOP_LOOKING_FOR_WIFI = '9',
    DO_DEMO_LEDS = 'A',
    STOP_FIREBASE = 'B',
    PLAYPAUSE = 'C',
    NEXT = 'D',
    TURN_OFF = 'E',
    ALL_PARAMETERS = 'F',
    STOP_CONNECT_MEETING = 'G',
    GET_BATTERY_LEVEL = 'H'
};
enum bt_messages_state
{
    FAILURE = '0',
    SUCCESS = '1'
};
enum bt_messages_sent
{
    WIFI_FAILURE = '0',
    WIFI_CONNECTED = '1'
};