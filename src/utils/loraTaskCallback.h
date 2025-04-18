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
  void setPole(const JsonObject &body);
  String serializeJsonPKG();
  void deserializeJsonPKG(String msg);
};

// AT NODE AFTER RECEIVE FROM GATEWAY
NodeStatus deserializeJsonFormat(const String &dataraw);

// AT GATEWAY WRAP TO SEND NODE
String serializeJsonFormat(String address, String method, String value);

// RECEIVE LORA CALLBACK
void LoRaRecvTask(void *pvParameters);

void controlRelay(String device, String state);

void controlPwm(String device, String value);

#endif