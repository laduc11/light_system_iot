#include <Arduino.h>
#include "globals.h"

#define PUBLISH_INTERVAL 3000 // 4 giây

unsigned long previousMillis = 0;
void setup()
{
  // for debug
  pinMode(INBUILD_LED_PIN, OUTPUT);

  Serial.begin(115200);
  Serial.println("ESP32 WROOM-32E Test");
  initWatchdogTimer(RESET_WATCHDOG_TIME);

  connect_init();
  delay(delay_for_initialization);
  device_init();

  // for initialize relayStatus
  bool relay_status = digitalRead(RELAY_PIN) == HIGH;
  JsonDocument relay_obj;
  relay_obj["switchstate"] = relay_status ? "high" : "low";
  char buffer[256];
  serializeJson(relay_obj, buffer);
  publishData(MQTT_SENDING_VALUE, buffer);
  Serial.println("Sending Initialize Switch Status");
}

void loop()
{
  unsigned long currentMillis = millis();

  if (currentMillis - previousMillis >= PUBLISH_INTERVAL)
  {
    previousMillis = currentMillis;

    int randomTemp = random(10, 100);

    JsonDocument doc;
    doc["illuminance"] = randomTemp;
    doc["longitude"] = LONGITUDE;
    doc["latitude"] = LATITUDE;
    char buffer[256];
    serializeJson(doc, buffer);
    publishData(MQTT_SENDING_VALUE, buffer);
  }
}