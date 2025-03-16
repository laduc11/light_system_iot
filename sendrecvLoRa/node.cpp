#include <Arduino.h>
#include <M5_LoRa_E220_JP.h>
#include <common.h>

#define LED 0x02

// DEFINE LORA
#define LORA_MAX_BUFFER_SIZE          32
#define LORA_MAX_WAITING_LIST         64  // This is the maximum element in waiting ack list
#define LORA_TIMER_FACTOR_MSG_RESEND  5
#define LORA_MAX_TIMES_RESEND         3

#define CONFIG_MODE_BAUD              9600

#define UART_LORA_RXD_PIN           3 // RX tren ESP 32
#define UART_LORA_TXD_PIN           1 // TX on ESP 32
#define UART_RXD_DEBUG_PIN          16 
#define UART_TXD_DEBUG_PIN          17 
#define TIME_CONFIGURE_PROCESS      1*1000
#define TIME_LORA_RECV_PROCESS      10

#define delay_lora_configure            (1000)
#define delay_for_initialization        (1000)
#define delay_rev_lora_process          (1)

LoRa_E220_JP lora;
struct LoRaConfigItem_t config;
// bool loraSend(String message);
RecvFrame_t data;
String data_buffer;

void toggleLED()
{
  if (digitalRead(LED) == HIGH) {
    digitalWrite(LED, LOW);
    Serial.println("LED OFF, S0");
  } else {
    digitalWrite(LED, HIGH);
    Serial.println("LED ON, S0");
  }
}

void LoRaRecvTask(void *pvParameters) {
  while (1)
  {
    if (lora.RecieveFrame(&data) == 0) {
      Serial.print("Data size: ");
      Serial.println(data.recv_data_len);
      data_buffer = "";
      for (uint16_t i = 0; i < data.recv_data_len; i++) {
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

      if (data_buffer == String(TEST_MESSAGE)) {
        toggleLED();
      } else {
        Serial.println("Received string not match");
      }
    } else {
      Serial.println("Receive data too large");
    }
    vTaskDelay(pdMS_TO_TICKS(delay_rev_lora_process));
  }
  vTaskDelete(nullptr);
}

void LoRaSendTask(void *pvParameters) {
  vTaskDelay(pdMS_TO_TICKS(delay_for_initialization));

  while (1)
  {
    String msg = "Xin chao nguoi dep.";
    // Call API from Iot server me dont know.
    // Sample code from nha san xuat
    // char msg[200] = {0};
    // ReadDataFromConsole(msg, (sizeof(msg) / sizeof(msg[0])));
    if (lora.SendFrame(config,(uint8_t*) msg.c_str(), msg.length()) == 0) {
      Serial.println("Send message success.");
      // notice to server fnction me dont know
    }
    else {
      Serial.println("Send message failed.");
      // notice to server function me dont know
    }
    Serial.flush();
    vTaskDelay(pdMS_TO_TICKS(delay_lora_configure));
  }
  
}

int temp = 0;
void loraSetup(void *pvParameters) {
  HardwareSerial* serial = (HardwareSerial*) pvParameters;
  // Set Serial1 for connecting to loRa
  // Initialize Serial communication
  lora.SetDefaultConfigValue(config);

  // Set initial configuration values
  config.own_address              = 0x0002;
  config.baud_rate                = BAUD_9600;
  config.air_data_rate            = BW125K_SF9;
  config.subpacket_size           = SUBPACKET_200_BYTE;
  config.rssi_ambient_noise_flag  = RSSI_AMBIENT_NOISE_ENABLE;
  config.transmitting_power       = TX_POWER_13dBm;
  config.own_channel              = 0x2A;
  config.rssi_byte_flag           = RSSI_BYTE_ENABLE;
  config.transmission_method_type = UART_P2P_MODE;
  config.lbt_flag                 = LBT_DISABLE;
  config.wor_cycle                = WOR_2000MS;
  config.encryption_key           = 0;
  config.target_address           = 0xffff;
  config.target_channel           = 0x2A;

  if (lora.InitLoRaSetting(config) != 0) {
    while (lora.InitLoRaSetting(config) != 0) {
      Serial.println("Lora init fail!");
      vTaskDelay(pdMS_TO_TICKS(delay_lora_configure));
    }
  }
  Serial.println("Lora init Success.");
}

void loraInit(void *pvParameters)
{
}

void binkLED(void *pvParam)
{
  while (1)
  {
    digitalWrite(LED, HIGH);
    Serial.println("LED ON, S0");
  
    vTaskDelay(pdMS_TO_TICKS(2000));
    digitalWrite(LED, LOW);
    Serial.println("LED OFF, S0");
    vTaskDelay(pdMS_TO_TICKS(2000));
  }
  
}

void setup() {

  Serial.begin(9600, SERIAL_8N1, UART_RXD_DEBUG_PIN, UART_TXD_DEBUG_PIN);
  Serial1.begin(9600, SERIAL_8N1, UART_LORA_RXD_PIN, UART_LORA_TXD_PIN);
  pinMode(LED, OUTPUT);
  lora.Init(&Serial1, CONFIG_MODE_BAUD, SERIAL_8N1, UART_LORA_RXD_PIN, UART_LORA_TXD_PIN);
  loraSetup(&Serial1);

  vTaskDelay(pdMS_TO_TICKS(delay_for_initialization));

  xTaskCreate(LoRaRecvTask, "UART receiver", 4096, nullptr, 0, nullptr);
}

void loop() {
  // put your main code here, to run repeatedly:
}