#ifndef __LORA_TASK_CALLBACK__
#define __LORA_TASK_CALLBACK__
#include "device/lora.h"
#include <globals.h>

class NodeStatus
{
public:
    int address;
    int state;
    int pwm_val;
    NodeStatus(int add = -1, int state = 0, int val = -1)
    {
    this->address = add;
    this->state = state;
    this->pwm_val = val;
    }
};

class Pole
{
public:
  uint16_t address;
  float temp;
  float humi;
  float intensity;
  Pole(uint16_t address = 0, float temp = 0.0, float humi = 0.0, float intensity = 0.0);
  void setPole(uint16_t address, float temp, float humi, float intensity);
  String serializeJsonPKG();
  void deserializeJsonPKG(String msg);
};

// AT NODE AFTER RECEIVE FROM GATEWAY
NodeStatus deserializeJsonFormat(const String &dataraw);

// AT GATEWAY WRAP TO SEND NODE
String serializeJsonFormat(String address, String method, String value);

// RECEIVE LORA CALLBACK
void LoRaRecvTask(void *pvParameters);

/*void LoRaSendTask(void *pvParameters)
{
  vTaskDelay(pdMS_TO_TICKS(delay_for_initialization));

  while (1)
  {
    String msg = "Xin chao nguoi dep.";
    if (lora.SendFrame(config, (uint8_t *)msg.c_str(), msg.length()) == 0)
    {
      Serial.println("Send message success.");
      // notice to server fnction me dont know
    }
    else
    {
      Serial.println("Send message failed.");
      // notice to server function me dont know
    }
    Serial.flush();
    vTaskDelay(pdMS_TO_TICKS(delay_lora_configure));
  }
}*/

void controlRelay(String device, String state);

void controlPwm(String device, String value);

#endif