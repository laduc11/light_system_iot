#ifndef LORA_H
#define LORA_H

#include "globals.h"

/**
 * @brief Module LoRa will use HardwareSerial Serial2 to communicate with ESP32
 * => Do not use at any other modules
 */

void loraInit();
void loraDelete();
bool loraSend(String message);
void loraReceiveCallback(void *pvParameter);
void getLoraDefaultConfig(Configuration *config);

// Lora Task
void sender(void *pvParameter);
void receiver(void *pvParameter);
void configLora(void *pvParameter);

#endif  // LORA_H