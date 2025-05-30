#ifndef LORA_H
#define LORA_H

#include "globals.h"

enum Role
{
    GATEWAY,
    NODE
};

void initLora();
void deinitLora();
void setConfiguration(Role role, uint16_t address);
LoRa_E220_JP* getLoraIns();
LoRaConfigItem_t* getConfigLora();

#endif  // LORA_H