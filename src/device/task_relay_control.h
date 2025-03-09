#ifndef RELAY_CONTROL_H
#define RELAY_CONTROL_H

#include <globals.h>

extern int relay_status;
void taskInitRelayControl(void *pvParameter);
void setRelayOn();
void setRelayOff();

#endif