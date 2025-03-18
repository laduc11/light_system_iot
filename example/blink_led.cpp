#include <globals.h>

void setup() 
{
    // Create task for RTOS
    xTaskCreate(taskLedBlink, "Task Blinky Led", 1024, nullptr, 2, nullptr);
}

void loop()
{
    // Do nothing
}