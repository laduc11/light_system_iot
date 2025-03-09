#include "globals.h"

#define PUBLISH_INTERVAL 3000 // 4 giây


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
  // Initialize device layer
  pinMode(INBUILD_LED_PIN, OUTPUT);
  Serial.begin(LORA_DEFAULT_BAUDRATE, SERIAL_8N1, UART_LORA_RXD_PIN, UART_LORA_TXD_PIN);
  Serial1.begin(UART_DEFAUT_BAUDRATE, SERIAL_8N1, UART_RXD_DEBUG_PIN, UART_TXD_DEBUG_PIN);
  
  // Initialize Network layer
  connect_init();
  
  // Initialize Device layer
  device_init();

  Serial.println("ESP32 WROOM-32E Test");
  initWatchdogTimer(RESET_WATCHDOG_TIME);

  delay(delay_for_initialization);    // Use synchronization techniques instead of wait a fixed duration
  sendCorrectDataToGateway();         // Just for testing

  // Create Task
  xTaskCreatePinnedToCore(taskLedBlink, "Task Blinky Led", 4096, NULL, 2, NULL, 1);
}

void loop()
{
  // Do nothing
}