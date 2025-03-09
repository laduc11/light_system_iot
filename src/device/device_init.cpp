#include "device_init.h"

void device_init()
{
    xTaskCreatePinnedToCore(task_pwm_light_control_init, "taskPWMLightControl", 4096, NULL, 1, NULL, 0);
    xTaskCreatePinnedToCore(taskLedBlink, "taskLedBlink", 4096, NULL, 2, NULL, 0);
    xTaskCreatePinnedToCore(taskInitRelayControl, "taskInitRelayControl", 4096, NULL, 1, NULL, 0);
}