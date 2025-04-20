#ifndef DHT20_H
#define DHT20_H

#include <globals.h>

void initDHT20();
void deinitDHT20();
void readDataDHT20(void *pvParameter);
void getDataDHT20(float &humidity, float &temperature);

#endif // DHT20_H