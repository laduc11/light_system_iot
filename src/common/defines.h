// DEFINE DEBUG MODE
#define _DEBUG_MODE_
// #define _IS_DEBUG_MODE_ true
#define _IS_DEBUG_MODE_ false

// DEFINE POLE INFORMATION
#define _ESP_NUMBER_ONE_
// #define _ESP_NUMBER_TWO_
// #define _ESP_NUMBER_THREE_
// #define _ESP_NUMBER_FOUR_
// #define _ESP_NUMBER_FIVE_

#ifdef _ESP_NUMBER_ONE_
#define DEVICE_ID "NEMA_0002"
#define STATION_ID "NEMA_0002"
#define STATION_NAME "NEMA_0002"
#endif

#ifdef _ESP_NUMBER_TWO_
#define DEVICE_ID "NEMA_0003"
#define STATION_ID "NEMA_0003"
#define STATION_NAME "NEMA_0003"
#endif

#ifdef _ESP_NUMBER_THREE_
#define DEVICE_ID "NEMA_0004"
#define STATION_ID "NEMA_0004"
#define STATION_NAME "NEMA_0004"
#endif

#ifdef _ESP_NUMBER_FOUR_
#define DEVICE_ID "NEMA_0005"
#define STATION_ID "NEMA_0005"
#define STATION_NAME "NEMA_0005"
#endif

#ifdef _ESP_NUMBER_FIVE_
#define DEVICE_ID "NEMA_0006"
#define STATION_ID "NEMA_0006"
#define STATION_NAME "NEMA_0006"
#endif

// DEFINE RELAY STATE
#define COUNT_UP true
#define COUNT_DOWN false

// DEFINE PIN
#define POLE_LED_PIN 5

// UART COMMON
#define UART_RXD_DEBUG_PIN 16
#define UART_TXD_DEBUG_PIN 17
#define UART_DEFAUT_BAUDRATE 9600

// DEFINE SETUP FOR PWM LEDC POLE LED
#define LEDC_CHANEL 0
#define FREQ_HZ 5000
#define RESOLUTION 13
#define INIT_DUTY_PWM 0
#define MAX_DUTY_PWM (1 << 13) - 1 // 2^13 -1

// DEFINE LED
#define INBUILD_LED_PIN 2
#define LED_ON HIGH
#define LED_OFF LOW

// DEFINE LORA
#define LORA_MAX_BUFFER_SIZE 32
#define LORA_MAX_WAITING_LIST 64 // This is the maximum element in waiting ack list
#define LORA_TIMER_FACTOR_MSG_RESEND 5
#define LORA_MAX_TIMES_RESEND 3
#define LORA_DEFAULT_BAUDRATE 9600

#define UART_LORA_TXD_PIN 1 // TX on ESP 32
#define UART_LORA_RXD_PIN 3 // RX tren ESP 32
#define TIME_CONFIGURE_PROCESS (1 * 1000)
#define TIME_LORA_RECV_PROCESS 10
// LORA SENDING PROCESSING STATUS

// DEFINE MQTT
#define MQTT_MAX_PACKAGE_SIZE 1024
#define MQTT_MAX_BUFFER_SIZE 128

#define MQTT_SERVER "app.coreiot.io"
#define MQTT_PORT 1883
#define MQTT_USERS "pole_1"
#define MQTT_PASSWORD "16012003"

#define MQTT_FEED_NOTHING ""
// #define MQTT_FEED_POLE_02 "BK_SmartPole/feeds/V20"
#define MQTT_SENDING_VALUE "v1/gateway/telemetry"
#define MQTT_REQUEST_TOPIC "v1/devices/me/rpc/request/+"
#define MQTT_FEED_TEST_LORA "BK_SmartPole/feeds/V5"
#define MQTT_FEED_TEST_LORA_SEND "BK_SmartPole/feeds/V4"
#define MQTT_FEED_TEST_MQTT "BK_SmartPole/feeds/V3"
#define MQTT_FEED_TEST_LORA_RECV "BK_SmartPole/feeds/V2"
#define MQTT_FEED_TEST_DIMMING "BK_SmartPole/Feeds/V1"

// DEFINE WIFI AP
#define WIFI_SSID "Fatcat"
#define WIFI_PASS "lnss2136"

// DEFINE DIMMING PWM
#define PWM_CHANNEL 1
#define PWM_FREQ 5000     // PWM frequency in Hz
#define PWM_RESOLUTION 13 // PWM resolution in bits
#define PWM_PIN 5

// DEFINE DELAY
#define delay_for_initialization (1000)
#define delay_wifi (3600 * 1000)
#define delay_connect (100)
#define delay_mqtt (100)
#define delay_send_message (60 * 1000)
#define delay_lora_configure (1000)
#define delay_led_blink (3000)
#define delay_rev_lora_process (1)
#define delay_send_lora_process (10 * 1000)
#define delay_lora_dummy_send (30 * 1000 + 13)
#define delay_lora_waiting_ack (60 * 1000 + 13)
#define delay_handle_mqtt_buffer (47)
#define delay_handle_lora_buffer (100)
#define delay_waiting_ack_process (1000)
#define delay_dimming_debug (100)
#define delay_process_buffer (10000)

// DEFINE WATCH DOG
#define RESET_WATCHDOG_TIME (120) // seconds

// DEFINE RELAY
#define RELAY_PIN 18
#define RELAY_ON HIGH
#define RELAY_OFF LOW

// DEFINE I2C
#define SDA_PIN 21
#define SCL_PIN 22
#define I2C_BAUDRATE    (100 * 1000)
#define I2C_READ_DATA_PERIOD (3000)      // Read data every 3 seconds

// DEFINE JSON MODE
#define ON_JSON 1
#define OFF_JSON 0

// DEFINE LONG LAT
const float LONGITUDE = 106.806;
const float LATITUDE = 10.8801;

// TOKEN GATEWAY
#define TOKEN_GATEWAY "I4pP9auSdf6KzYU0PmWV"