#include "device_init.h"

void device_init()
{
    pwm_init();
    initRelayControl();
}