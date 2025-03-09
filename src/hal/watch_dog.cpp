#include "watch_dog.h"

// Initialize the watchdog timer
void initWatchdogTimer(int timeoutSeconds)
{
    esp_task_wdt_init(timeoutSeconds, true);
    printlnData(MQTT_FEED_NOTHING, "Watchdog timer initialized.");
}

// Add a task to the watchdog timer
void addTaskToWatchdog(TaskHandle_t taskHandle)
{
    esp_task_wdt_add(taskHandle);
    printlnData(MQTT_FEED_NOTHING, "Task added to watchdog timer.");
}

// Reset the watchdog timer
void resetWatchdog()
{
    esp_task_wdt_reset();
}

// Remove a task from the watchdog timer
void removeTaskFromWatchdog(TaskHandle_t taskHandle)
{
    esp_task_wdt_delete(taskHandle);
    printlnData(MQTT_FEED_NOTHING, "Task removed from watchdog timer.");
}