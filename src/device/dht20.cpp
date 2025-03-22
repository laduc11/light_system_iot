#include "dht20.h"

void initDHT20()
{
    Wire.begin(21, 22); // For I2C DHT20
    DHT20 *dht = new DHT20(&Wire);
    while (!dht->begin())
    {
      Serial.println("Failed to initialize DHT20 sensor!");
      delay(100);   // Wait 100ms and retry to connect
    }
    Serial.println("DHT20 initialized successfully.");
}

void readDataDHT20(void *pvParameter)
{

}