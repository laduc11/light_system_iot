#ifndef TASK_MQTT_H
#define TASK_MQTT_H

#include "globals.h"

void taskMQTT(void *pvParameters);
bool publishData(String feedName, String massage);

#endif