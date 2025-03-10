#ifndef PRINT_DEBUG_H
#define PRINT_DEBUG_H

#include "globals.h"

void initDebugSerial(HardwareSerial *serial = &Serial);
void printData(String message = "");
void printlnData(String message = "");

HardwareSerial * getDebugSerial();

#endif // PRINT_DEBUG_H