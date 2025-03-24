#include "dht20.h"

static DHT20 *dht_ptr = nullptr;
static bool is_data_avaible = false;

void initDHT20()
{
  if (nullptr != dht_ptr)
  {
    delete dht_ptr;
  }
  Wire.begin(SDA_PIN, SCL_PIN, I2C_BAUDRATE); // For I2C DHT20
  dht_ptr = new DHT20(&Wire);
  while (!dht_ptr->begin())
  {
    Serial.println("Failed to initialize DHT20 sensor!");
    vTaskDelay(pdMS_TO_TICKS(500));   // Wait 500ms and retry to connect
  }
  Serial.println("DHT20 initialized successfully.");
}

void deinitDHT20()
{
  if (nullptr != dht_ptr) {
    delete dht_ptr;
  }
}

void readDataDHT20(void *pvParameter)
{
  if (nullptr == dht_ptr) {
    printlnData("DHT20 is not initialized");
  }

  while (true)
  {
    // Get current time
    TickType_t now = xTaskGetTickCount();

    // READ DATA
    int status = dht_ptr->read();
    if (DHT20_OK == status)
    {
      is_data_avaible = true;
    }

#ifdef DEBUG_DHT20
    // Print to Serial
    Serial.print("DHT20 \t");
    //  DISPLAY DATA, sensor has only one decimal.
    Serial.print(dht_ptr->getHumidity(), 1);
    Serial.print("\t\t");
    Serial.print(dht_ptr->  (), 1);
    Serial.print("\t\t");

    // Handle status
    switch (status)
    {
    case DHT20_OK:
      Serial.print("OK");
      break;
    case DHT20_ERROR_CHECKSUM:
      Serial.print("Checksum error");
      break;
    case DHT20_ERROR_CONNECT:
      Serial.print("Connect error");
      break;
    case DHT20_MISSING_BYTES:
      Serial.print("Missing bytes");
      break;
    case DHT20_ERROR_BYTES_ALL_ZERO:
      Serial.print("All bytes read zero");
      break;
    case DHT20_ERROR_READ_TIMEOUT:
      Serial.print("Read time out");
      break;
    case DHT20_ERROR_LASTREAD:
      Serial.print("Error read too fast");
      break;
    default:
      Serial.print("Unknown error");
      break;
    }
    Serial.println();
#endif  // DEBUG_DHT20
    vTaskDelayUntil(&now, pdMS_TO_TICKS(I2C_READ_DATA_PERIOD));
  }
}

inline void getDataDHT20(float &humidity, float &temperature)
{
  if (nullptr == dht_ptr || !is_data_avaible)
  {
    humidity = -1.0f;
    temperature = -1.0f;
    return;
  }
  humidity = dht_ptr->getHumidity();
  temperature = dht_ptr->getTemperature();
  is_data_avaible = false;
}
