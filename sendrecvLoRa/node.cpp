#include "globals.h"

// Handle after receive lora package from gateway
void handleProcessBuffer(void *pvParameters)
{
  BasicQueue<String> *q = (BasicQueue<String> *)pvParameters;
  while(1)
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
      } 
    }
    vTaskDelay(pdMS_TO_TICKS(delay_process_buffer));
  }
  vTaskDelete(nullptr);
}

// void LoRaSendTask(void *pvParameters)
// {
//   vTaskDelay(pdMS_TO_TICKS(delay_for_initialization));

//   while (1)
//   {
//     String msg = "Xin chao nguoi dep.";
//     if (lora.SendFrame(config, (uint8_t *)msg.c_str(), msg.length()) == 0)
//     {
//       Serial.println("Send message success.");
//       // notice to server fnction me dont know
//     }
//     else
//     {
//       Serial.println("Send message failed.");
//       // notice to server function me dont know
//     }
//     Serial.flush();
//     vTaskDelay(pdMS_TO_TICKS(delay_lora_configure));
//   }
// }

/* Setup function */
void setup()
{
  // Initialize Pin and Serial
  Serial.begin(UART_DEFAUT_BAUDRATE, SERIAL_8N1, UART_RXD_DEBUG_PIN, UART_TXD_DEBUG_PIN);
  initDebugSerial(&Serial);
  pinMode(INBUILD_LED_PIN, OUTPUT);
  digitalWrite(INBUILD_LED_PIN, LOW);   // Turn off the build-in LED

  // Initialize watchdog
  initWatchdogTimer(RESET_WATCHDOG_TIME);

  // Initialize DHT20
  initDHT20();

  // Initialize LoRa
  initLora();
  setConfiguration(NODE, 0x0002);   // Hard code with address node: 0x0002

  // Initialize Network layer and Device layer
  device_init();
  BasicQueue<String> *buffer = new BasicQueue<String>();
  // Create task for RTOS
  xTaskCreate(handleProcessBuffer, "handle process buffer", 1024 * 8, buffer, 1, nullptr);
  xTaskCreate(LoRaRecvTask, "rcv", 1024*4, buffer, 0, nullptr);
  xTaskCreate(readDataDHT20, "DHT20 data reader", 1024 * 4, nullptr, 1, nullptr);

  digitalWrite(INBUILD_LED_PIN, HIGH);    // Turn on the LED when set up completely
}

/* Loop function */
void loop()
{
  // put your main code here, to run repeatedly:
}