#include <globals.h>

#define MAX_RETRIES 5
#define INTERVAL_TIME 2000 // unit: ms

RecvFrame_t data;
String data_buffer;

uint32_t start_time;

// ACK attributes
uint8_t counter_wait;

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
        if (start_time == 0)
        {
          start_time = millis();
        }
      }

      if (method == "setPWM")
      {
        printData("Check device: ");
        printlnData(device);

        // Code for sending messag to adjust pwm value with LoRa to node
        controlPwm(device, params);
        if (start_time == 0)
        {
          start_time = millis();
        }
      }
    }
    vTaskDelay(pdMS_TO_TICKS(200));
  }
  vTaskDelete(nullptr);
}

// Function to send fake data to Sever CoreIOT
void sendCorrectDataToGateway()
{
  JsonDocument jsonDoc;

  const char *devices[] = {"SmartPole 001", "SmartPole 002", "SmartPole 003"};
  const char *switchStates[] = {"low", "low", "low"};

  for (int i = 0; i < 3; i++)
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

// Publish sensor value, from lora -> gateway -> server
void processNodePkg(const NodeStatus &node)
{
  if (node.pwm_val >= 0)
  {
    // Handle dimming control
    // pwm_val = -1: Unchanged
    JsonDocument doc;
    char pole_addr[15];
    sprintf(pole_addr, "SmartPole %03x", node.address);
    JsonArray telemetryArray = doc[pole_addr].to<JsonArray>();
    JsonObject telemetryObject = telemetryArray.createNestedObject();
    telemetryObject["pwm_value"] = String(node.pwm_val);
    String msg;
    serializeJson(doc, msg);
    publishData(MQTT_GATEWAY_TELEMETRY_TOPIC, msg);
    printlnData("Updating PWM to telemetry for device");
  }
  else
  {
    // Handle Relay control
    // node.state == -1: Unchanged
    if (node.state == 1)
    {
      JsonDocument doc;
      char pole_addr[15];
      sprintf(pole_addr, "SmartPole %03x", node.address);
      JsonObject data = doc[String(pole_addr)].to<JsonObject>();
      data["switchstate"] = "ON";
      String msg;
      serializeJson(doc, msg);
      publishData(MQTT_GATEWAY_ATTRIBUTES_TOPIC, msg);
      printlnData("Updating Relay state for device");
    }
    else if (node.state == 0)
    {
      JsonDocument doc;
      char pole_addr[15];
      sprintf(pole_addr, "SmartPole %03x", node.address);
      JsonObject data = doc[String(pole_addr)].to<JsonObject>();
      data["switchstate"] = "OFF";
      String msg;
      serializeJson(doc, msg);
      publishData(MQTT_GATEWAY_ATTRIBUTES_TOPIC, msg);
      printlnData("Updating Relay state for device");
    }
  }
}

void processPolePkg(const Pole &pole)
{
  JsonDocument doc;
  char pole_addr[15];
  sprintf(pole_addr, "SmartPole %03x", pole.address);
  JsonArray telemetryArray = doc[pole_addr].to<JsonArray>();
  JsonObject telemetryObject = telemetryArray.createNestedObject();
  telemetryObject["humidity"] = String(pole.humi);
  telemetryObject["intensity"] = String(pole.intensity);
  telemetryObject["temperature"] = String(pole.temp);
  String msg;
  serializeJson(doc, msg);
  publishData(MQTT_GATEWAY_TELEMETRY_TOPIC, msg);
  printlnData("Updating Sensor Value to telemetry for device");
  return;
}

void handleProcessBuffer(void *pvParameters)
{
  BasicQueue<String> *q = (BasicQueue<String> *)pvParameters;
  // ngoay nhanh ve xoa sau
  while (1)
  {
    if (!q->isEmpty())
    {
      String msg = q->pop();
      JsonDocument doc;
      DeserializationError error = deserializeJson(doc, msg);
      if (error)
      {
        printData("[ERROR] deserializeJson() failed: ");
        printlnData(error.f_str());
        continue;
      }

      String header = doc["Header"].as<String>();
      if (header == "ACK_STT")
      {
        printlnData("Start processing Node Command Package");
        NodeStatus node = deserializeJsonFormat(msg);
        if (start_time > 0)
        {
          uint32_t duration = millis() - start_time;
          Serial.printf("Interval time: %d ms\n", duration);
          Serial.printf("Retry: %d times\n", counter_wait);

          // Reset attribute for retry mechanism
          start_time = 0;
          counter_wait = 0;
        }
        processNodePkg(node);
      }
      else if (header == "PERIOD_SS")
      {
        printlnData("Start processing Pole Sensor Package");
        Pole pole;
        pole.deserializeJsonPKG(msg);
        processPolePkg(pole);
      }
      else
      {
        printlnData("Wrong header package.");
      }
    }
    vTaskDelay(pdMS_TO_TICKS(delay_process_buffer));
  }
  vTaskDelete(nullptr);
}

void setup()
{
  start_time = 0;
  counter_wait = 0;

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
  BasicQueue<String> *buffer_N2G = new BasicQueue<String>();

  printlnData("ESP32 WROOM-32E Gateway");
  initWatchdogTimer(RESET_WATCHDOG_TIME);

  vTaskDelay(pdMS_TO_TICKS(5000));

  sendCorrectDataToGateway();

  // Create Task
  xTaskCreate(handleProcessBufferS2G, "handle message from server", 1024 * 4, buffer_S2G, 1, nullptr);
  xTaskCreate(handleProcessBuffer, "handle message from node", 1024 * 4, buffer_N2G, 1, nullptr);
  xTaskCreate(LoRaRecvTask, "rcv", 1024 * 4, buffer_N2G, 0, nullptr);
  xTaskCreate(taskLedBlink, "Task Blinky Led", 1024, nullptr, 2, nullptr);
}

void loop()
{
  // put your main code here, to run repeatedly:

  // Need to create the task instead of main loop
  if (start_time > 0)
  {
    uint32_t duration = millis() - start_time;
    if (duration > INTERVAL_TIME)
    {
      counter_wait++;
      start_time = millis();
      // Send the control message again
      /* Hardcode */
      controlRelay("SmartPole 003", "toggle");
    }
  }
  if (counter_wait >= MAX_RETRIES)
  {
    printlnData("Timeout for ACK message");
    counter_wait = 0;
    start_time = 0;
  }

  vTaskDelay(pdMS_TO_TICKS(INTERVAL_TIME / 4));
}