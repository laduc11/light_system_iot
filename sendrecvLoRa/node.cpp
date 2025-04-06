#include "globals.h"

class NodeStatus {
public:
  int address;
  int state;
  int pwm_val;
  NodeStatus(int add = -1, int state = 0, int val = -1)
  {
    this->address = add;
    this->state = state;
    this->pwm_val = val;
  }
};

NodeStatus deserializeJsonFormat(const String &dataraw)
{
  NodeStatus node;
  JsonDocument doc;
  DeserializationError error = deserializeJson(doc, dataraw);

  if (error)
  {
      printData("[ERROR] deserializeJson() failed: ");
      printlnData(error.f_str());
      return node;
  }

  // get value from Json message
  String address = doc["Address"].as<String>(); // _001 -> 1
  JsonObject data = doc["data"].as<JsonObject>();
  String state = data["Relay"].as<String>();
  String pwm_val = data["PWM"].as<String>();
  
  // Pass value to Node's attributes
  node.address = address.toInt();
  node.pwm_val = pwm_val.toInt();
  if (state == "high") // do server gui ve qui dinh high/low
    node.state = 1;
  if (state == "low")
    node.state = 0;

  Serial.println("Deserialize data success.");
  return node;
}

void LoRaRecvTask(void *pvParameters)
{
  String data_buffer;   // Simple buffer for received data
  RecvFrame_t data;
  Serial.println("Waiting for new Package coming ...");
  while (1)
  {
    if (getLoraIns()->RecieveFrame(&data) == 0)
    {
      data_buffer = String(data.recv_data, data.recv_data_len);
      Serial.print("Data size: ");
      Serial.println(data.recv_data_len);
      Serial.print(data_buffer);

      Serial.println();
      Serial.printf("RSSI: %d dBm\n", data.rssi);
      Serial.flush();

      NodeStatus node = deserializeJsonFormat(data_buffer);
      // Check destination address
      if (node.address != getConfigLora()->own_address)
      {
        Serial.println("Not equal to onw address.");
        continue;
      }

      if (node.pwm_val >= 0) 
      {
        // Handle dimming control
        // pwm_val = -1: Unchanged
        pwm_set_duty(node.pwm_val);
      }
      else 
      {
        // Handle Relay control
        // node.state == -1: Unchanged
        if (node.state == 1) 
        {
          setRelayOn();
          digitalWrite(INBUILD_LED_PIN, HIGH);
        }
        else if (node.state == 0)
        {
          setRelayOff();
          digitalWrite(INBUILD_LED_PIN, LOW);
        }
      } 
    }
  }
  vTaskDelete(nullptr);
}

// void LoRaSendTask(void *pvParameters)
// {
//   vTaskDelay(pdMS_TO_TICKS(delay_for_initialization));

//   while (1)
//   {
//     String msg = "Xin chao nguoi dep.";
//     if (lora.SendFrame(config, (uint8_t *)msg.c_str(), msg.length()) == 0)
//     {
//       Serial.println("Send message success.");
//       // notice to server fnction me dont know
//     }
//     else
//     {
//       Serial.println("Send message failed.");
//       // notice to server function me dont know
//     }
//     Serial.flush();
//     vTaskDelay(pdMS_TO_TICKS(delay_lora_configure));
//   }
// }


/* Setup function */
void setup()
{
  // Initialize Pin and Serial
  Serial1.begin(UART_DEFAUT_BAUDRATE, SERIAL_8N1, UART_LORA_RXD_PIN, UART_LORA_TXD_PIN);
  initDebugSerial(&Serial);
  pinMode(INBUILD_LED_PIN, OUTPUT);
  digitalWrite(INBUILD_LED_PIN, LOW);   // Turn off the build-in LED

  // Initialize watchdog
  initWatchdogTimer(RESET_WATCHDOG_TIME);

  // Initialize DHT20
  initDHT20();

  // Initialize LoRa
  initLora();
  setConfiguration(NODE, 0x0002);   // Hard code with address node: 0x0002

  // Initialize Network layer and Device layer
  device_init();

  // Create task for RTOS
  xTaskCreate(LoRaRecvTask, "Test Node receive", 1024 * 8, nullptr, 0, nullptr);
  xTaskCreate(readDataDHT20, "DHT20 data reader", 1024 * 4, nullptr, 1, nullptr);

  digitalWrite(INBUILD_LED_PIN, HIGH);    // Turn on the LED when set up completely
}

/* Loop function */
void loop()
{
  // put your main code here, to run repeatedly:
}