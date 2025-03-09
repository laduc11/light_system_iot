#include "lora.h"

/*
  // REG 0-1
  0x0000,  // own_address 0

  // REG 2
  0b011,    // baud_rate 9600 bps
  0b10000,  // air_data_rate SF:9 BW:125

  // REG 3
  0b00,  // subpacket_size 200
  0b1,   // rssi_ambient_noise_flag enable
  0b01,  // transmitting_power 13 dBm

  // REG 4
  0x00,  // own_channel 0

  // REG 5
  0b1,    // rssi_byte_flag enable
  0b1,    // transmission_method_type p2p
  0b0,    // lbt_flag 有効
  0b011,  // wor_cycle 2000 ms

  // REG 6-7
  0x0000,  // encryption_key 0

  // LOCAL CONFIG FOR SEND API
  0x0000,  // target_address 0
  0x00     // target_channel 0
*/

LoRa_E220_JP lora;
struct LoRaConfigItem_t config;

/**
 * @brief 
 * 
 */
void loraInit()
{
  lora.Init(&Serial2, LORA_DEFAULT_BAUDRATE, SERIAL_8N1, UART_LORA_RXD_PIN, UART_LORA_TXD_PIN);

  // Set config
  lora.SetDefaultConfigValue(config);
  config.own_address = 0x1234;
  config.own_channel = 0x01;
#ifdef  LORA_CONFIG_MODE
  lora.InitLoRaSetting(config);
#endif  // LORA_CONFIG_MODE
}

/**
 * @brief 
 * 
 * @param pvParameter 
 */
void loraReceiveCallback(void *pvParameter)
{
  HardwareSerial *serial = (HardwareSerial *)pvParameter;
  uint8_t buffer_uart[LORA_MAX_BUFFER_SIZE];
  uint32_t stop;
  while (1) {
    stop = 0;
    // Send dump data to LoRa
    uint8_t buffer_write[3] = {0xc1, 0x00, 0x02};
    serial->write(buffer_write, 3);

    Serial.printf("Sending data size: %ld\r\n", 3);
    for (uint32_t i = 0; i < 3; ++i) {
      Serial.printf("%02X ", buffer_write[i]);
    }
    Serial.println();
    vTaskDelay(pdMS_TO_TICKS(1000));

    uint32_t size = 10;
    // Reset buffer
    for (uint32_t i = 0; i < LORA_MAX_BUFFER_SIZE; ++i) {
      buffer_uart[i] = 0;
    }
    // Read data from UART
    stop = serial->readBytes(buffer_uart, size);

    Serial.printf("Recieved data size: %ld\r\n", stop);
    for (uint32_t i = 0; i < stop; ++i) {
      Serial.printf("%02X ", buffer_write[i]);
    }
    Serial.println();
    serial->flush();
    vTaskDelay(pdMS_TO_TICKS(1000));
  }
}