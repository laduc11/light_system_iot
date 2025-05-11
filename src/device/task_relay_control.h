#ifndef RELAY_CONTROL_H
#define RELAY_CONTROL_H

#include <globals.h>

extern int relay_status;
void taskInitRelayControl(void *pvParameter);
void initRelayControl();
void setRelayOn();
void setRelayOff();
void toggleRelay();
int getRelayStatus();

#endif