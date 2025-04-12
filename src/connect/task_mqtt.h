#ifndef TASK_MQTT_H
#define TASK_MQTT_H

#include "globals.h"
#include "utils/basic_queue.h"

extern BasicQueue<String> *buffer_S2G;

void taskMQTT(void *pvParameters);
bool publishData(String feedName, String massage);

#endif