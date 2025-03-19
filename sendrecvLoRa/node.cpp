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

      // Extract fields: device_id, method, value
      extractFields(data_buffer, device_id, method, value);
      // if (data_buffer == String("test_4: high"))
      // {
      //   digitalWrite(INBUILD_LED_PIN, HIGH);
      //   Serial.println("LED ON, S0");
      // }
      // else if (data_buffer == String("test_4: low"))
      // {
      //   digitalWrite(INBUILD_LED_PIN, LOW);
      //   Serial.println("LED OFF, S0");
      // }
      // else
      // {
      //   Serial.println("Received string not match");
      // }
      if (device_id)
      {
        if (method == "Relay")
        {
          if (value == "high")
          {
            setRelayOn();
          }
          else if (value == "low")
          {
            setRelayOff();
          }
        }
        else if (method == "PWM")
        {
          pwm_set_duty(int(value.toInt()));
        }
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

void setup()
{

  Serial.begin(9600, SERIAL_8N1, UART_RXD_DEBUG_PIN, UART_TXD_DEBUG_PIN);
  Serial1.begin(9600, SERIAL_8N1, UART_LORA_RXD_PIN, UART_LORA_TXD_PIN);
  pinMode(INBUILD_LED_PIN, OUTPUT);
  lora.Init(&Serial1, LORA_DEFAULT_BAUDRATE, SERIAL_8N1, UART_LORA_RXD_PIN, UART_LORA_TXD_PIN);
  loraSetup(&Serial1);

  vTaskDelay(pdMS_TO_TICKS(delay_for_initialization));

  xTaskCreate(LoRaRecvTask, "UART receiver", 4096, nullptr, 0, nullptr);
}

void loop()
{
  // put your main code here, to run repeatedly:
}