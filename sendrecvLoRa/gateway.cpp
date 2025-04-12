#include <globals.h>

RecvFrame_t data;
String data_buffer;

void handleProcessBufferS2G(void *pvParameters)
{
  BasicQueue<String> *q = (BasicQueue<String> *)pvParameters;
  while (1)
  {
    /* code */
    if (!q->isEmpty())
    {
      JsonDocument doc;
      String message = q->pop();
      DeserializationError error = deserializeJson(doc, message);

      if (error)
      {
        printData("[ERROR] deserializeJson() failed: ");
        printlnData(error.f_str());
        return;
      }

      // Get value from package receivce from server
      String device = doc["device"].as<String>();
      JsonObject data = doc["data"].as<JsonObject>();
      String method = data["method"].as<String>();
      String params = data["params"].as<String>();

      // Print for debug--
      printData("Method: ");
      printlnData(method);
      printData("Params: ");
      printlnData(params);
      //-------------------

      if (method == "setState")
      {
        printData("Check device: ");
        printlnData(device);
        // Code for sending message to control relay with LoRa to node
        controlRelay(device, params);

        // Publish message to server to synchronous state of device
        JsonDocument jsonDoc;

        JsonArray deviceArray = jsonDoc[device].to<JsonArray>();
        JsonObject statusObj = deviceArray.add<JsonObject>();
        statusObj["switchstate"] = params;

        char buffer[512];
        serializeJson(jsonDoc, buffer, sizeof(buffer));

        publishData(MQTT_GATEWAY_ATTRIBUTES_TOPIC, buffer);

        printlnData("Updating Relay state for device");
        printlnData(buffer);
      }

      if (method == "setPWM")
      {
        printData("Check device: ");
        printlnData(device);

        // Code for sending messag to adjust pwm value with LoRa to node
        controlPwm(device, params);
        // Publish message to server to synchronous state of device


        printlnData("Updating PWM value for device");
      }
    }
    vTaskDelay(pdMS_TO_TICKS(2000));
  }
  vTaskDelete(nullptr);
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

  publishData(MQTT_GATEWAY_TELEMETRY_TOPIC, buffer);

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

  // Init receive queue
  BasicQueue<String> * buffer_N2G = new BasicQueue<String>();

  printlnData("ESP32 WROOM-32E Gateway");
  initWatchdogTimer(RESET_WATCHDOG_TIME);

  vTaskDelay(pdMS_TO_TICKS(5000));

  sendCorrectDataToGateway();

  // Create Task
  xTaskCreate(handleProcessBufferS2G, "abc" , 1024*4, buffer_S2G, 1, nullptr);
  xTaskCreate(LoRaRecvTask, "rcv", 1024*4, buffer_N2G, 0, nullptr);
  xTaskCreate(taskLedBlink, "Task Blinky Led", 1024, nullptr, 2, nullptr);
}

void loop() {
  // put your main code here, to run repeatedly:
}