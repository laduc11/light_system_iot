#include <Arduino.h>
#include "globals.h"

#define PUBLISH_INTERVAL 3000 // 4 giây

unsigned long previousMillis = 0;

void sendCorrectDataToGateway()
{
  JsonDocument jsonDoc;

  const char *devices[] = {"test_2", "test_3", "test_4"};
  const char *switchStates[] = {"high", "low", "low"};

  for (int i = 0; i < 3; i++)
  {
    JsonArray deviceArray = jsonDoc[devices[i]].to<JsonArray>();
    JsonObject statusObj = deviceArray.add<JsonObject>();
    statusObj["switchstate"] = switchStates[i];
  }

  char buffer[512];
  serializeJson(jsonDoc, buffer, sizeof(buffer));

  publishData(MQTT_SENDING_VALUE, buffer);

  Serial.println("Sending Data to Gateway:");
  Serial.println(buffer);
}

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
  sendCorrectDataToGateway();

}

void loop()
{
  // unsigned long currentMillis = millis();

  // if (currentMillis - previousMillis >= PUBLISH_INTERVAL)
  // {
  //   previousMillis = currentMillis;

  //   int randomTemp = random(10, 100);

  //   JsonDocument doc;
  //   doc["illuminance"] = randomTemp;
  //   doc["longitude"] = LONGITUDE;
  //   doc["latitude"] = LATITUDE;
  //   char buffer[256];
  //   serializeJson(doc, buffer);
  //   publishData(MQTT_SENDING_VALUE, buffer);
  // }
}