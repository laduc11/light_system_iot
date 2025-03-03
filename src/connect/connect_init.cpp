#include "connect_init.h"

void connect_init()
{
    xTaskCreatePinnedToCore(taskWifi, "taskWifi", 4096, NULL, 3, NULL, 0);
    xTaskCreatePinnedToCore(taskMQTT, "taskMQTT", 4096, NULL, 1, NULL, 0);
}