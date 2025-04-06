#include <globals.h>

RecvFrame_t data;
String data_buffer;

void LoRaRecvTask(void *pvParameters) {
  vTaskDelay(pdMS_TO_TICKS(delay_for_initialization));
  LoRa_E220_JP *lora_ptr = getLoraIns();
  while (1)
  {
    Serial.println("before");
    lora_ptr->RecieveFrame(&data);
    Serial.println("after");
    
    if (lora_ptr->RecieveFrame(&data) == 0) {
      data_buffer = "";
      for (uint16_t i = 0; i < data.recv_data_len; i++) {
        data_buffer += (char)data.recv_data[i];
        Serial.printf("%c", data.recv_data[i]);
      }
      Serial.println();
      Serial.println("HEX dump:");
      for (uint16_t i = 0; i < data.recv_data_len; i++)
      {
        Serial.printf("%02x ", data.recv_data[i]);
      }
      Serial.println();
      Serial.printf("RSSI: %d dBm\n", data.rssi);
      Serial.flush();
      // lora_buffer.push_back(textBuffer);
    }
    vTaskDelay(pdMS_TO_TICKS(delay_rev_lora_process));
  }
}

// Function to send fake data to Sever CoreIOT
void sendCorrectDataToGateway()
{
  JsonDocument jsonDoc;

  const char *devices[] = {"SmartPole 001", "SmartPole 002"};
  const char *switchStates[] = {"low", "low"};

  for (int i = 0; i < 2; i++)
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

void setup() {

  Serial.begin(UART_DEFAUT_BAUDRATE, SERIAL_8N1, UART_RXD_DEBUG_PIN, UART_TXD_DEBUG_PIN);
  initDebugSerial(&Serial);

  // Initialize LoRa
  initLora();
  setConfiguration(GATEWAY, 0x0001);

  // Initialize Network layer
  connect_init();
  
  // Initialize Device layer
  device_init();

  printlnData("ESP32 WROOM-32E Gateway");
  initWatchdogTimer(RESET_WATCHDOG_TIME);

  vTaskDelay(pdMS_TO_TICKS(delay_for_initialization));

  sendCorrectDataToGateway();

  // Create Task
  xTaskCreate(taskLedBlink, "Task Blinky Led", 1024, nullptr, 2, nullptr);
}

void loop() {
  // put your main code here, to run repeatedly:
}