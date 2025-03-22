#ifndef DEVICE_H
#define DEVICE_H

#include <globals.h>
#include "task_pwm_nema.h"
#include "task_inbuild_led.h"
#include "task_relay_control.h"
#include "lora.h"
#include "dht20.h"

void device_init();

#endif // DEVICE_H