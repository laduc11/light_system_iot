#include "task_pwm_nema.h"

int dutyPercentManually = 0;

void pwm_init()
{
    // Configure LEDC Timer
    ledcSetup(PWM_CHANNEL, PWM_FREQ, PWM_RESOLUTION);

    // Attach the channel to the GPIO to be controlled
    ledcAttachPin(POLE_LED_PIN, PWM_CHANNEL);

    // Initialize fade service for smooth fading
    ledcWrite(PWM_CHANNEL, INIT_DUTY_PWM);
}

void pwm_set_duty(int dutyPercent)
{
    if (dutyPercent > 100)
    {
        dutyPercent = 100;
    }
    else if (dutyPercent < 0)
    {
        dutyPercent = 0;
    }
    dutyPercentManually = map(dutyPercent, 0, 100, 0, pow(2, PWM_RESOLUTION) - 1);

    ledcWrite(PWM_CHANNEL, dutyPercentManually);
    Serial.println("Success set PWM");
}
void task_pwm_light_control_init(void *pvParameters)
{
    pwm_init();

    vTaskDelete(NULL);
}