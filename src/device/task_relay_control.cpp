#include "task_relay_control.h"

int relay_status = RELAY_ON;

void taskInitRelayControl(void *pvParameter)
{
    pinMode(RELAY_PIN, OUTPUT);
    relay_status = RELAY_OFF;
    digitalWrite(RELAY_PIN, RELAY_OFF);

    vTaskDelete(NULL);
}

void initRelayControl()
{
    pinMode(RELAY_PIN, OUTPUT);
    relay_status = RELAY_OFF;
    digitalWrite(RELAY_PIN, RELAY_OFF);
}

void setRelayOn()
{
    digitalWrite(RELAY_PIN, RELAY_ON);
    printlnData("Turn relay on");
}

void setRelayOff()
{
    digitalWrite(RELAY_PIN, RELAY_OFF);
    printlnData("Turn relay off");
}