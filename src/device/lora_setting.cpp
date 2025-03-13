#include "lora.h"

void getLoraDefaultConfig(Configuration *config) {
    config->ADDL = 0x12;  // First part of address
    config->ADDH = 0x34; // Second part

    config->CHAN = 1; // Communication channel

    config->SPED.uartBaudRate = UART_BPS_9600; // Serial baud rate
    config->SPED.airDataRate = AIR_DATA_RATE_010_24; // Air baud rate
    config->SPED.uartParity = MODE_00_8N1; // Parity bit

    config->OPTION.subPacketSetting = SPS_200_00; // Packet size
    config->OPTION.RSSIAmbientNoise = RSSI_AMBIENT_NOISE_DISABLED; // Need to send special command
    config->OPTION.transmissionPower = POWER_22; // Device power

    config->TRANSMISSION_MODE.enableRSSI = RSSI_DISABLED; // Enable RSSI info
    config->TRANSMISSION_MODE.fixedTransmission = FT_FIXED_TRANSMISSION; // Enable repeater mode
    config->TRANSMISSION_MODE.enableLBT = LBT_DISABLED; // Check interference
    config->TRANSMISSION_MODE.WORPeriod = WOR_2000_011; // WOR timing

    config->CRYPT.CRYPT_H = 0x00;
    config->CRYPT.CRYPT_L = 0x00;
}