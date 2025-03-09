#ifndef LORA_H
#define LORA_H

#include "globals.h"

void loraInit();
bool loraSend(String message);
void loraReceiveCallback(void *pvParameter);

#endif  // LORA_H