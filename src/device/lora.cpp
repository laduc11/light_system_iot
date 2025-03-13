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

/* Private attribute for module LoRa */
LoRa_E220 *lora_ = nullptr;
Configuration config_;

/* Private function */
/**
 * @brief Print configuration for debugging
 * 
 * @param config 
 */
void printConfiguration(Configuration *config)
{
	getDebugSerial()->println("----------------------------------------");

	getDebugSerial()->print(F("HEAD : "));  getDebugSerial()->print(config->COMMAND, HEX);getDebugSerial()->print(" ");getDebugSerial()->print(config->STARTING_ADDRESS, HEX);getDebugSerial()->print(" ");getDebugSerial()->println(config->LENGHT, HEX);
	getDebugSerial()->println(F(" "));
	getDebugSerial()->print(F("AddH : "));  getDebugSerial()->println(config->ADDH, HEX);
	getDebugSerial()->print(F("AddL : "));  getDebugSerial()->println(config->ADDL, HEX);
	getDebugSerial()->println(F(" "));
	getDebugSerial()->print(F("Chan : "));  getDebugSerial()->print(config->CHAN, DEC); getDebugSerial()->print(" -> "); getDebugSerial()->println(config->getChannelDescription());
	getDebugSerial()->println(F(" "));
	getDebugSerial()->print(F("SpeedParityBit     : "));  getDebugSerial()->print(config->SPED.uartParity, BIN);getDebugSerial()->print(" -> "); getDebugSerial()->println(config->SPED.getUARTParityDescription());
	getDebugSerial()->print(F("SpeedUARTDatte     : "));  getDebugSerial()->print(config->SPED.uartBaudRate, BIN);getDebugSerial()->print(" -> "); getDebugSerial()->println(config->SPED.getUARTBaudRateDescription());
	getDebugSerial()->print(F("SpeedAirDataRate   : "));  getDebugSerial()->print(config->SPED.airDataRate, BIN);getDebugSerial()->print(" -> "); getDebugSerial()->println(config->SPED.getAirDataRateDescription());
	getDebugSerial()->println(F(" "));
	getDebugSerial()->print(F("OptionSubPacketSett: "));  getDebugSerial()->print(config->OPTION.subPacketSetting, BIN);getDebugSerial()->print(" -> "); getDebugSerial()->println(config->OPTION.getSubPacketSetting());
	getDebugSerial()->print(F("OptionTranPower    : "));  getDebugSerial()->print(config->OPTION.transmissionPower, BIN);getDebugSerial()->print(" -> "); getDebugSerial()->println(config->OPTION.getTransmissionPowerDescription());
	getDebugSerial()->print(F("OptionRSSIAmbientNo: "));  getDebugSerial()->print(config->OPTION.RSSIAmbientNoise, BIN);getDebugSerial()->print(" -> "); getDebugSerial()->println(config->OPTION.getRSSIAmbientNoiseEnable());
	getDebugSerial()->println(F(" "));
	getDebugSerial()->print(F("TransModeWORPeriod : "));  getDebugSerial()->print(config->TRANSMISSION_MODE.WORPeriod, BIN);getDebugSerial()->print(" -> "); getDebugSerial()->println(config->TRANSMISSION_MODE.getWORPeriodByParamsDescription());
	getDebugSerial()->print(F("TransModeEnableLBT : "));  getDebugSerial()->print(config->TRANSMISSION_MODE.enableLBT, BIN);getDebugSerial()->print(" -> "); getDebugSerial()->println(config->TRANSMISSION_MODE.getLBTEnableByteDescription());
	getDebugSerial()->print(F("TransModeEnableRSSI: "));  getDebugSerial()->print(config->TRANSMISSION_MODE.enableRSSI, BIN);getDebugSerial()->print(" -> "); getDebugSerial()->println(config->TRANSMISSION_MODE.getRSSIEnableByteDescription());
	getDebugSerial()->print(F("TransModeFixedTrans: "));  getDebugSerial()->print(config->TRANSMISSION_MODE.fixedTransmission, BIN);getDebugSerial()->print(" -> "); getDebugSerial()->println(config->TRANSMISSION_MODE.getFixedTransmissionDescription());


	getDebugSerial()->println("----------------------------------------");
}

/*<===============================================================================================================================================================>*/
/**
 * @brief Initialize instance LoRa and binding with Serial2
 * 
 */
void loraInit()
{
  lora_ = new LoRa_E220(UART_LORA_TXD_PIN, UART_LORA_RXD_PIN, &Serial2, -1, -1, -1, UART_BPS_RATE_9600, SERIAL_8N1);
  getLoraDefaultConfig(&config_);
}

/**
 * @brief Release all memory
 * 
 */
void loraDelete()
{
  delete lora_;
}

/**
 * @brief Config LoRa task
 * 
 * @param pvParameter 
 */
void configLora(void *pvParameter)
{
  if (nullptr != lora_) {
    lora_->setConfiguration(config_, WRITE_CFG_PWR_DWN_SAVE);

    Configuration config;
    ResponseStructContainer rc = lora_->getConfiguration();
    printConfiguration((Configuration *)rc.data);
  }
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

    if (nullptr != getDebugSerial()) {
      getDebugSerial()->printf("Sending data size: %ld\r\n", 3);
    }
    for (uint32_t i = 0; i < 3; ++i) {
      if (nullptr != getDebugSerial()) {
        getDebugSerial()->printf("%02X ", buffer_write[i]);
      }
    }
    printlnData();
    vTaskDelay(pdMS_TO_TICKS(1000));

    uint32_t size = 10;
    // Reset buffer
    for (uint32_t i = 0; i < LORA_MAX_BUFFER_SIZE; ++i) {
      buffer_uart[i] = 0;
    }
    // Read data from UART
    stop = serial->readBytes(buffer_uart, size);

    if (nullptr != getDebugSerial()) {
      getDebugSerial()->printf("Recieved data size: %ld\r\n", stop);
    }
    for (uint32_t i = 0; i < stop; ++i) {
      if (nullptr != getDebugSerial()) {
        getDebugSerial()->printf("%02X ", buffer_write[i]);
      }
    }
    printlnData();
    serial->flush();
    vTaskDelay(pdMS_TO_TICKS(1000));
  }
}