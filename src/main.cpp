#include <Arduino.h>
#include <M5_LoRa_E220_JP.h>

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

#define delay_lora_configure            (60*1000)
#define delay_for_initialization        (10*1000)
#define delay_rev_lora_process          (1)
LoRa_E220_JP lora;
struct LoRaConfigItem_t config;
int temp = 0;
bool loraSend(String message);

void taskLoraInit(void *pvParameters) {

  // Initialize Serial communication
  lora.SetDefaultConfigValue(config);

  // Set initial configuration values
  config.own_address              = 0x0002;
  config.baud_rate                = BAUD_9600;
  config.air_data_rate            = BW125K_SF9;
  config.subpacket_size           = SUBPACKET_200_BYTE;
  config.rssi_ambient_noise_flag  = RSSI_AMBIENT_NOISE_ENABLE;
  config.transmitting_power       = TX_POWER_13dBm;
  config.own_channel              = 0x00;
  config.rssi_byte_flag           = RSSI_BYTE_ENABLE;
  config.transmission_method_type = UART_P2P_MODE;
  config.lbt_flag                 = LBT_DISABLE;
  config.wor_cycle                = WOR_2000MS;
  config.encryption_key           = 0x00;
  config.target_address           = 0xFFFF;
  config.target_channel           = 0x00;

  if (lora.InitLoRaSetting(config) != 0) {
    Serial.println("Lora init fail!");
    temp--;
    // while (lora.InitLoRaSetting(config) != 0) {
    //   vTaskDelay(delay_lora_configure);
    // }
  }
  Serial.println("Lora init Success.");
  temp++;
  vTaskDelete(nullptr);
}
RecvFrame_t data;
String textbuffer;
void taskLoraRecv(void *pvParameters) {
  vTaskDelay(delay_for_initialization);

  while(1) {
    if (lora.RecieveFrame(&data) == 0) {
      textbuffer = "";
      for (uint16_t i = 0; i < data.recv_data_len; i++) {
        textbuffer += (char)data.recv_data[i];
      }
      // lora_buffer.push_back(textBuffer);
    }
    vTaskDelay(delay_rev_lora_process);
  }
}
void taskLoraSend(void *pvParameters) {
  vTaskDelay(delay_for_initialization);

  String msg = "Xin chao nguoi dep.";
  while (1)
  {
    loraSend(msg);
    vTaskDelay(delay_lora_configure);
  }
  
}
bool loraSend(String message) {
  if (lora.SendFrame(config, (uint8_t *)(message.c_str()), message.length()) == 0) {
    Serial.println("Send success.");
    return true;
  }
  else
  {
    Serial.println("Send fail.");
    return false;
  }
  
}

void loraInit(void *pvParameters)
{
  xTaskCreate(taskLoraInit, "lora Init", 4096, NULL, 1, NULL);
}

#define MAX_BUFFER  10

void uart_CB(void* pvparam) {
  HardwareSerial *serial = (HardwareSerial *)pvparam;
  uint8_t buffer_uart[MAX_BUFFER];
  uint32_t stop;
  while (1) {
    stop = 0;
    // Send dump data to Serial1 (LoRa)
    uint8_t buffer_write[3] = {0xc1, 0x00, 0x02};
    serial->write(buffer_write, 3);

    Serial.printf("Sending data size: %ld\r\n", 3);
    for (uint32_t i = 0; i < 3; ++i) {
      Serial.print(buffer_write[i], HEX);
    }
    Serial.println();
    vTaskDelay(pdMS_TO_TICKS(1000));

    uint32_t size = 10;
    // Reset buffer
    for (uint32_t i = 0; i < MAX_BUFFER; ++i) {
      buffer_uart[i] = 0;
    }
    // Read data from UART
    stop = serial->readBytes(buffer_uart, size);

    Serial.printf("Recieved data size: %ld\r\n", stop);
    for (uint32_t i = 0; i < stop; ++i) {
      Serial.print(buffer_uart[i], HEX);
    }
    Serial.println();
    serial->flush();
    vTaskDelay(pdMS_TO_TICKS(1000));
  }
}

void binkLED(void *pvParam)
{
  digitalWrite(LED, HIGH);
  Serial.println("LED ON, S0");

  vTaskDelay(pdMS_TO_TICKS(2000));
  digitalWrite(LED, LOW);
  Serial.println("LED OFF, S0");
  vTaskDelay(pdMS_TO_TICKS(2000));
}

void setup() {

  Serial.begin(115200, SERIAL_8N1, UART_RXD_DEBUG_PIN, UART_TXD_DEBUG_PIN);
  Serial1.begin(9600, SERIAL_8N1, UART_LORA_RXD_PIN, UART_LORA_TXD_PIN);
  pinMode(LED, OUTPUT);
  
  xTaskCreate(uart_CB, "UART callback", 4096, &Serial1, 1, nullptr);
  // xTaskCreate(binkLED, "Blinky LED", 4096, nullptr, 1, nullptr);
  // vTaskStartScheduler();
  // xTaskCreatePinnedToCore(uart_CB, "UART callback", 4096, nullptr, 3, nullptr, 0);
  // xTaskCreate(uart_CB, "UART callback", 4096, nullptr, 3, 0);
  // lora.Init(&Serial1, 9600, SERIAL_8N1, UART_LORA_TXD_PIN, UART_LORA_RXD_PIN);
  // Serial.print("1\n");
  // xTaskCreatePinnedToCore(taskLoraInit, "taskLora", 4096, NULL, 1, NULL, 0);
  // Serial.print("2\n");
  // xTaskCreatePinnedToCore(taskLoraRecv, "taskLoraRecv", 4096, NULL, 0, NULL, 1);
  // xTaskCreatePinnedToCore(taskLoraSend, "taskLoraSend", 4096, NULL, 1, NULL,1);
}

void loop() {
  // put your main code here, to run repeatedly:
}