#ifndef TASK_PWM_NEMA_H
#define TASK_PWM_NEMA_H

#include "globals.h"

void pwm_init();
void pwm_set_duty(int dutyPercent);
void task_pwm_light_control_init(void *pvParameters);
void task_pwm_light_control_update(void *pvParameter);

#endif