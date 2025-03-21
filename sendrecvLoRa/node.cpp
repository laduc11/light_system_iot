#include "globals.h"

LoRa_E220_JP lora;
struct LoRaConfigItem_t config;
// bool loraSend(String message);
RecvFrame_t data;
String data_buffer;

// Function to extract device_id, method, and value from data_buffer
int device_id;
String method;
String value;
// Function to extract device_id, method, and value from data_buffer
void extractFields(String data_buffer, int &device_id, String &method, String &value)
{
  // Check if the string contains "test_2" or "test_4"
  if (data_buffer.indexOf("test_2") >= 0 || data_buffer.indexOf("test_4") >= 0)
  {
    // Extract device_id from "test_2" or "test_4"
    device_id = (data_buffer.indexOf("test_2") >= 0) ? 2 : 4;

    // Extract method ("Relay" or "PWM")
    if (data_buffer.indexOf("Relay") >= 0)
    {
      method = "Relay";

      // Extract value of Relay ("high" or "low")
      int relayPos = data_buffer.indexOf("Relay: ");
      if (relayPos >= 0)
      {
        value = data_buffer.substring(relayPos + 7); // Get value after "Relay: "

        // Remove any trailing whitespace or '}' character
        size_t pos = value.indexOf('}');
        if (pos != -1)
        {                                  // Replace String::npos with -1
          value = value.substring(0, pos); // Truncate after '}'
        }
      }
    }
    else if (data_buffer.indexOf("PWM") >= 0)
    {
      method = "PWM";

      // Extract PWM value (number)
      int pwmPos = data_buffer.indexOf("PWM: ");
      if (pwmPos >= 0)
      {
        value = data_buffer.substring(pwmPos + 5); // Get value after "PWM: "
      }
    }
  }
  else
  {
    device_id = -1;
    method = "Invalid";
    value = "Invalid";
  }
}

enum CONTROL{
  Unknown,
  Relay,
  Pwm
};

bool DeserializeData(const String &dataraw)
// SmartPole 001 { Relay: high/low } || SmartPole 001 { PWM: 50 }
// 012345678901234567890123456789012    0123456789012345678901234
{
  // Precheck string
  if (!dataraw.startsWith("SmartPole"))
  {
    Serial.println("Receive wrong format message SmartPole.");
    return false;
  }
  else
  {
    if (!config.own_address == dataraw[12]) return false;
  }
  Serial.println("Correct address, processing package ....");
  // Pre-check done, xu ly data
  String content = dataraw.substring(16, dataraw.lastIndexOf(' '));
  //Relay: high/low_||PWM: 50_
  CONTROL flag = Unknown;
  if (content.startsWith("Relay:")) flag = Relay;
  else flag = Pwm;
  String value = content.substring(content.indexOf(' ') + 1, content.lastIndexOf(' ') - 1);
  if (flag == Relay)
  {
    if (value.startsWith("high")) 
    {
      setRelayOn();
      return true;
    }
    if (value.startsWith("low")) 
    {
      setRelayOff();
      return true;
    }
    Serial.println("Receive wrong format message of Relay control.");
    return false;
  }
  if (flag == Pwm)
  {
    int duty = value.toInt();
    pwm_set_duty(duty);
    return true;
  }
  Serial.println("Receive wrong format message, not include Relay or Pwm.");
  return false;
}

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
  Serial.println("check point");
  while (1)
  {
    if (lora.RecieveFrame(&data) == 0)
    {
      Serial.print("Data size: ");
      Serial.println(data.recv_data_len);
      data_buffer = "";
      for (uint16_t i = 0; i < data.recv_data_len; i++)
      {
        data_buffer += (char)data.recv_data[i];
        Serial.printf("%c", data.recv_data[i]);
      }
      Serial.println();
      // Serial.println("HEX dump:");
      // for (uint16_t i = 0; i < data.recv_data_len; i++)
      // {
      //   Serial.printf("%02x ", data.recv_data[i]);
      // }
      // Serial.println();
      Serial.printf("RSSI: %d dBm\n", data.rssi);
      Serial.flush();

      if (data_buffer == String("test_4: high"))
      {
        digitalWrite(INBUILD_LED_PIN, HIGH);
        Serial.println("LED ON, S0");
      }
      else if (data_buffer == String("test_4: low"))
      {
        digitalWrite(INBUILD_LED_PIN, LOW);
        Serial.println("LED OFF, S0");
      }
      else
      {
        Serial.println("Received string not match");
      }
    }
    else
    {
      Serial.println("Receive data too large");
    }
    vTaskDelay(pdMS_TO_TICKS(delay_rev_lora_process));
  }
  vTaskDelete(nullptr);
}

void LoRaSendTask(void *pvParameters)
{
  vTaskDelay(pdMS_TO_TICKS(delay_for_initialization));

  while (1)
  {
    String msg = "Xin chao nguoi dep.";
    if (lora.SendFrame(config, (uint8_t *)msg.c_str(), msg.length()) == 0)
    {
      Serial.println("Send message success.");
      // notice to server fnction me dont know
    }
    else
    {
      Serial.println("Send message failed.");
      // notice to server function me dont know
    }
    Serial.flush();
    vTaskDelay(pdMS_TO_TICKS(delay_lora_configure));
  }
}

int temp = 0;
void loraSetup(void *pvParameters)
{
  HardwareSerial *serial = (HardwareSerial *)pvParameters;
  // Set Serial1 for connecting to loRa
  // Initialize Serial communication
  lora.SetDefaultConfigValue(config);

  // Set initial configuration values
  config.own_address = 0x0002;
  config.baud_rate = BAUD_9600;
  config.air_data_rate = BW125K_SF9;
  config.subpacket_size = SUBPACKET_200_BYTE;
  config.rssi_ambient_noise_flag = RSSI_AMBIENT_NOISE_ENABLE;
  config.transmitting_power = TX_POWER_13dBm;
  config.own_channel = 0x00;
  config.rssi_byte_flag = RSSI_BYTE_ENABLE;
  config.transmission_method_type = UART_P2P_MODE;
  config.lbt_flag = LBT_DISABLE;
  config.wor_cycle = WOR_2000MS;
  config.encryption_key = 0;
  config.target_address = 0xffff;
  config.target_channel = 0x00;

  if (lora.InitLoRaSetting(config) != 0)
  {
    while (lora.InitLoRaSetting(config) != 0)
    {
      Serial.println("Lora init fail!");
      vTaskDelay(pdMS_TO_TICKS(delay_lora_configure));
    }
  }
  Serial.println("Lora init Success.");
}

void loraInit(void *pvParameters)
{
}

void readDHT20(void *pvParam)
{
  vTaskDelay(pdMS_TO_TICKS(1000));
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

void setup()
{

  Serial.begin(9600, SERIAL_8N1, UART_RXD_DEBUG_PIN, UART_TXD_DEBUG_PIN);
  Serial1.begin(9600, SERIAL_8N1, UART_LORA_RXD_PIN, UART_LORA_TXD_PIN);
  pinMode(INBUILD_LED_PIN, OUTPUT);
  initDebugSerial(&Serial);

  // Initialize DHT20
  Wire.begin(21, 22); // For I2C DHT20
  DHT20 *dht = new DHT20(&Wire);
  if (!dht->begin())
  {
    Serial.println("Failed to initialize DHT20 sensor!");
    // while (1);
  }
  Serial.println("DHT20 initialized successfully.");
  delay(2000);

  // lora.Init(&Serial1, LORA_DEFAULT_BAUDRATE, SERIAL_8N1, UART_LORA_RXD_PIN, UART_LORA_TXD_PIN);
  // loraSetup(&Serial1);
  initWatchdogTimer(RESET_WATCHDOG_TIME);
  connect_init();
  device_init();

  xTaskCreate(readDHT20, "dht20", 4096, dht, 1, nullptr);
  vTaskDelay(pdMS_TO_TICKS(delay_for_initialization));
  xTaskCreate(sendCorrectDataToGateway, "send", 4096, dht, 1, nullptr);

  // xTaskCreate(LoRaRecvTask, "UART receiver", 4096, nullptr, 0, nullptr);
}

void loop()
{
  // put your main code here, to run repeatedly:
}