#include "task_inbuild_led.h"

void taskLedBlink(void *pvParameters)
{
    pinMode(INBUILD_LED_PIN, OUTPUT);

    while (1)
    {
      digitalWrite(INBUILD_LED_PIN, HIGH);
      Serial.println("LED ON, S0");
    
      vTaskDelay(pdMS_TO_TICKS(2000));
      digitalWrite(INBUILD_LED_PIN, LOW);
      Serial.println("LED OFF, S0");
      vTaskDelay(pdMS_TO_TICKS(2000));
    }
    vTaskDelete(nullptr);
}

void toggleLED()
{
  if (digitalRead(INBUILD_LED_PIN) == HIGH)
  {
    digitalWrite(INBUILD_LED_PIN, LOW);
    Serial.println("LED OFF, S0");
  }
  else
  {
    digitalWrite(INBUILD_LED_PIN, HIGH);
    Serial.println("LED ON, S0");
  }
}