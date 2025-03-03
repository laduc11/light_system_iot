#ifndef WATCH_DOG_H
#define WATCH_DOG_H

#include <globals.h>

void initWatchdogTimer(int timeoutSeconds);
void addTaskToWatchdog(TaskHandle_t taskHandle);
void resetWatchdog();
void removeTaskFromWatchdog(TaskHandle_t taskHandle);

#endif // WATCH_DOG_H