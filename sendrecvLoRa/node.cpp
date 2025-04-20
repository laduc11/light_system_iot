#include "globals.h"

// Handle after receive lora package from gateway
void handleProcessBuffer(void *pvParameters)
{
  BasicQueue<String> *q = (BasicQueue<String> *)pvParameters;
  while (1)
  {
    if (!q->isEmpty())
    {
      NodeStatus node;
      node = deserializeJsonFormat(q->pop());
      // Check destination address
      if (node.address != getConfigLora()->own_address)
      {
        Serial.println("Not equal to onw address.");
        continue;
      }

      if (node.pwm_val >= 0)
      {
        // Handle dimming control
        // pwm_val = -1: Unchanged
        pwm_set_duty(node.pwm_val);
        String msg = serializeJsonFormat(String(getConfigLora()->own_address), "PWM", String(node.pwm_val));
        if (getLoraIns()->SendFrame(*(getConfigLora()), (uint8_t *)msg.c_str(), msg.length()) == 0)
          printlnData("Send message confirm set pwm to GTW");
      }
      else
      {
        // Handle Relay control
        // node.state == -1: Unchanged
        if (node.state == 1)
        {
          setRelayOn();
          digitalWrite(INBUILD_LED_PIN, HIGH);
        }
        else if (node.state == 0)
        {
          setRelayOff();
          digitalWrite(INBUILD_LED_PIN, LOW);
        }
        else
        {
          if (digitalRead(RELAY_PIN))
          {
            digitalWrite(INBUILD_LED_PIN, LOW);
            setRelayOff();
            String msg = serializeJsonFormat(String(getConfigLora()->own_address), "Relay", "low");
            if (getLoraIns()->SendFrame(*(getConfigLora()), (uint8_t *)msg.c_str(), msg.length()) == 0)
              printlnData("Send message relay LOW to GTW");
          }
          else
          {
            digitalWrite(INBUILD_LED_PIN, HIGH);
            setRelayOn();
            String msg = serializeJsonFormat(String(getConfigLora()->own_address), "Relay", "high");
            if (getLoraIns()->SendFrame(*(getConfigLora()), (uint8_t *)msg.c_str(), msg.length()) == 0)
              printlnData("Send message relay HIGH to GTW");
          }
        }
      }
    }
    vTaskDelay(pdMS_TO_TICKS(delay_process_buffer));
  }
  vTaskDelete(nullptr);
}

// Dinh ky gui ve gateway gia tri cam bien
void updatePeriodPole(void *pvParameters)
{
  while (1)
  {
    Pole pole;
    pole.address = getConfigLora()->own_address;
    getDataDHT20(pole.humi, pole.temp);
    // pole.humi = (rand() % 100) * 1.0;
    // pole.temp = (rand() % 100) *1.0;
    // pole.intensity = analogRead(A10);`
    pole.intensity = 50.00;
    String pkg = pole.serializeJsonPKG();
    if (getLoraIns()->SendFrame(*(getConfigLora()), (uint8_t *)pkg.c_str(), pkg.length()) == 0)
    {
      Serial.printf("Upload Pole value to GW: Humid: %f, Temp: %f", pole.humi, pole.temp);
      Serial.println();
    }
    else
      Serial.println("Upload fail.");
    delay(delay_sending_period_value);
  }
}

/* Setup function */
void setup()
{
  // Initialize Pin and Serial
  Serial.begin(UART_DEFAUT_BAUDRATE, SERIAL_8N1, UART_RXD_DEBUG_PIN, UART_TXD_DEBUG_PIN);
  initDebugSerial(&Serial);
  pinMode(INBUILD_LED_PIN, OUTPUT);
  digitalWrite(INBUILD_LED_PIN, LOW); // Turn off the build-in LED

  // Initialize watchdog
  initWatchdogTimer(RESET_WATCHDOG_TIME);

  // Initialize DHT20
  initDHT20();

  // Initialize LoRa
  initLora();
  setConfiguration(NODE, 0x0003); // Hard code with address node: 0x0003

  // Initialize Network layer and Device layer
  device_init();
  BasicQueue<String> *buffer = new BasicQueue<String>();
  // Create task for RTOS
  xTaskCreate(handleProcessBuffer, "handle process buffer", 1024 * 8, buffer, 1, nullptr);
  xTaskCreate(LoRaRecvTask, "rcv", 1024 * 8, buffer, 0, nullptr);
  xTaskCreate(readDataDHT20, "DHT20 data reader", 1024 * 4, nullptr, 1, nullptr);
  xTaskCreate(updatePeriodPole, "Update period sensor val", 1024 * 10, nullptr, 5, nullptr);
  digitalWrite(INBUILD_LED_PIN, HIGH); // Turn on the LED when set up completely
}

/* Loop function */
void loop()
{
  // put your main code here, to run repeatedly:
}

// { "SmartPole 001": {"switchstate": "ON"}} - > v1/gateway/attributes