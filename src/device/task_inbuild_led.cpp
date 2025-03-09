#include "task_inbuild_led.h"

int ledState = LED_OFF;

void taskLedBlink(void *pvParameters)
{
    pinMode(INBUILD_LED_PIN, OUTPUT);

    while (true)
    {
        if (ledState == LED_OFF)
        {
            digitalWrite(INBUILD_LED_PIN, LED_ON);
            ledState = LED_ON;
        }
        else
        {
            digitalWrite(INBUILD_LED_PIN, LED_OFF);
            ledState = LED_OFF;
        }
        vTaskDelay(delay_led_blink);
    }
}