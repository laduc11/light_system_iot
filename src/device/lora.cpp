#include "lora.h"

// Private  instance
LoRa_E220_JP *lora_ptr = nullptr;
LoRaConfigItem_t *configuration_ptr = nullptr;

// Private function
LoRaConfigItem_t *getGatewayConfiguration(uint16_t address)
{
    LoRaConfigItem_t *configuration = (LoRaConfigItem_t *)malloc(sizeof(LoRaConfigItem_t));
    configuration->own_address              = address;
    configuration->baud_rate                = BAUD_9600;
    configuration->air_data_rate            = BW125K_SF9;
    configuration->subpacket_size           = SUBPACKET_200_BYTE;
    configuration->rssi_ambient_noise_flag  = RSSI_AMBIENT_NOISE_ENABLE;
    configuration->transmitting_power       = TX_POWER_13dBm;
    configuration->own_channel              = 0x00;
    configuration->rssi_byte_flag           = RSSI_BYTE_ENABLE;
    configuration->transmission_method_type = UART_P2P_MODE;
    configuration->lbt_flag                 = LBT_DISABLE;
    configuration->wor_cycle                = WOR_2000MS;
    configuration->encryption_key           = 0;
    configuration->target_address           = 0xffff;
    configuration->target_channel           = 0x00;
    return configuration;
}

LoRaConfigItem_t *getNodeConfiguration(uint16_t address)
{
    LoRaConfigItem_t *configuration = (LoRaConfigItem_t *)malloc(sizeof(LoRaConfigItem_t));
    configuration->own_address              = address;
    configuration->baud_rate                = BAUD_9600;
    configuration->air_data_rate            = BW125K_SF9;
    configuration->subpacket_size           = SUBPACKET_200_BYTE;
    configuration->rssi_ambient_noise_flag  = RSSI_AMBIENT_NOISE_ENABLE;
    configuration->transmitting_power       = TX_POWER_13dBm;
    configuration->own_channel              = 0x00;
    configuration->rssi_byte_flag           = RSSI_BYTE_ENABLE;
    configuration->transmission_method_type = UART_P2P_MODE;
    configuration->lbt_flag                 = LBT_DISABLE;
    configuration->wor_cycle                = WOR_2000MS;
    configuration->encryption_key           = 0;
    configuration->target_address           = 0xffff;
    configuration->target_channel           = 0x00;
    return configuration;
}


// Interface

void initLora()
{
    if (lora_ptr) {
        free(lora_ptr);
    }
    lora_ptr = (LoRa_E220_JP *)malloc(sizeof(LoRa_E220_JP));
    lora_ptr->Init(&Serial1, LORA_DEFAULT_BAUDRATE, SERIAL_8N1, UART_LORA_RXD_PIN, UART_LORA_TXD_PIN);

    
}

void deinitLora()
{
    if (lora_ptr) free(lora_ptr);
    if (configuration_ptr)free(configuration_ptr);
}

void setConfiguration(Role role, uint16_t address)
{
    if (role == NODE) 
    {
        configuration_ptr = getNodeConfiguration(address);
    } 
    else if (role ==GATEWAY) 
    {
        configuration_ptr = getGatewayConfiguration(address);
    } 
    else
    {
        Serial.println("Role unknown");
        return;
    }
    while (lora_ptr->InitLoRaSetting(*configuration_ptr) != 0)
    {
        Serial.println("Lora init fail!");
        vTaskDelay(pdMS_TO_TICKS(delay_lora_configure));
    }
    Serial.println("Lora init success!");
}

LoRa_E220_JP* getLoraIns()
{
    return lora_ptr;
}

LoRaConfigItem_t* getConfigLora()
{
    return configuration_ptr;
}
