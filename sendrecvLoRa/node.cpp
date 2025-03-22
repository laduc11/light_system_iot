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

void readDHT20(void *pvParam)
{
  DHT20 *dht = (DHT20 *)pvParam;
  uint8_t count = 0;
  while (true)
  {
    // READ DATA
    int status = dht->read();
    if (count % 5 == 0)
    {
      count = 0;
      Serial.print('\n');
      Serial.println("Type\tHumidity (%)\tTemp (°C)\tStatus");
    }
    count++;
    //
    // Print to Serial
    Serial.print("DHT20 \t");
    //  DISPLAY DATA, sensor has only one decimal.
    Serial.print(dht->getHumidity(), 1);
    String a(dht->getHumidity(), 1);
    Serial.print("\t\t");
    Serial.print(dht->getTemperature(), 1);
    Serial.print("\t\t");
    switch (status)
    {
    case DHT20_OK:
      Serial.print("OK");
      break;
    case DHT20_ERROR_CHECKSUM:
      Serial.print("Checksum error");
      break;
    case DHT20_ERROR_CONNECT:
      Serial.print("Connect error");
      break;
    case DHT20_MISSING_BYTES:
      Serial.print("Missing bytes");
      break;
    case DHT20_ERROR_BYTES_ALL_ZERO:
      Serial.print("All bytes read zero");
      break;
    case DHT20_ERROR_READ_TIMEOUT:
      Serial.print("Read time out");
      break;
    case DHT20_ERROR_LASTREAD:
      Serial.print("Error read too fast");
      break;
    default:
      Serial.print("Unknown error");
      break;
    }
    Serial.println();

    vTaskDelay(pdMS_TO_TICKS(3000));
  }
}

void sendCorrectDataToGateway(void *pvParam)
{
  DHT20 *dht = (DHT20 *)pvParam;
  while (1)
  {
    JsonDocument jsonDoc;

    const char *devices[] = {"test_2"};
    uint8_t relayState = digitalRead(RELAY_PIN);
    String switchState;
    if (relayState == HIGH)
    {
      switchState = "high";
    }
    else
    {
      switchState = "low";
    }

    for (int i = 0; i < 1; i++)
    {
      JsonArray deviceArray = jsonDoc[devices[i]].to<JsonArray>();
      JsonObject statusObj = deviceArray.add<JsonObject>();
      statusObj["switchstate"] = switchState;
      statusObj["humidity"] = String(dht->getHumidity(), 1);
      statusObj["temperature"] = String(dht->getTemperature(), 1);
    }

    char buffer[512];
    serializeJson(jsonDoc, buffer, sizeof(buffer));

    publishData(MQTT_SENDING_VALUE, buffer);

    printlnData("Sending Data to Gateway:");
    printlnData(buffer);
    vTaskDelay(pdMS_TO_TICKS(5000));
  }
}

/* Setup function */
void setup()
{
  // Initialize Pin and Serial
  Serial.begin(9600, SERIAL_8N1, UART_RXD_DEBUG_PIN, UART_TXD_DEBUG_PIN);
  Serial1.begin(9600, SERIAL_8N1, UART_LORA_RXD_PIN, UART_LORA_TXD_PIN);
  pinMode(INBUILD_LED_PIN, OUTPUT);
  digitalWrite(INBUILD_LED_PIN, LOW);
  initDebugSerial(&Serial);

  // Initialize watchdog
  initWatchdogTimer(RESET_WATCHDOG_TIME);

  // Initialize DHT20
  // initDHT20();

  // Initialize LoRa
  initLora();
  setConfiguration(NODE, 0x0002);   // Hard code with address node: 0x0002

  // Initialize Network layer and Device layer
  device_init();

  // Create task for RTOS
  xTaskCreate(LoRaRecvTask, "Test Node receive", 1024*8, nullptr, 0, nullptr);
  // xTaskCreate(readDHT20, "dht20", 4096, dht, 1, nullptr);

  digitalWrite(INBUILD_LED_PIN, HIGH);

}

/* Loop function */
void loop()
{
  // put your main code here, to run repeatedly:
}