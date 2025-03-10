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

  printlnData("Sending Data to Gateway:");
  printlnData(buffer);
}



void setup()
{
  // Intialize for debug
  Serial.begin(UART_DEFAUT_BAUDRATE, SERIAL_8N1, UART_RXD_DEBUG_PIN, UART_TXD_DEBUG_PIN);
  initDebugSerial(&Serial);

  // Initialize device layer
  pinMode(INBUILD_LED_PIN, OUTPUT);
  
  // Initialize Network layer
  connect_init();
  
  // Initialize Device layer
  device_init();
  loraInit();

  printlnData("ESP32 WROOM-32E Test");
  initWatchdogTimer(RESET_WATCHDOG_TIME);

  delay(delay_for_initialization);    // Use synchronization techniques instead of wait a fixed duration
  sendCorrectDataToGateway();         // Just for testing

  // Create Task
  xTaskCreatePinnedToCore(taskLedBlink, "Task Blinky Led", 4096, NULL, 2, NULL, 1);
  xTaskCreatePinnedToCore(loraReceiveCallback, "Task LoRa Callback", 4096, &Serial2, 2, NULL, 1);
}

void loop()
{
  // Do nothing
}