#include "task_wifi.h"

void taskWifi(void *pvParameters)
{
    addTaskToWatchdog(NULL);
    WiFi.mode(WIFI_STA);

    Serial.print("Connecting to SSID: ");
    Serial.println(WIFI_SSID);

    WiFi.begin(WIFI_SSID, WIFI_PASS);

    int wifiRetryCount = 0;
    while (WiFi.status() != WL_CONNECTED && wifiRetryCount < 20)
    {
        Serial.printf("Attempt %d: WiFi Status = %d\n", wifiRetryCount + 1, WiFi.status());
        resetWatchdog();
        vTaskDelay(pdMS_TO_TICKS(1000));
        wifiRetryCount++;
    }

    if (WiFi.status() != WL_CONNECTED)
    {
        Serial.println("[ERROR] WiFi connection failed! Restarting in 5s...");
        vTaskDelay(pdMS_TO_TICKS(5000));
        ESP.restart();
    }

    Serial.println("[INFO] WiFi Connected!");
    Serial.print("IP Address: ");
    Serial.println(WiFi.localIP());

    while (true)
    {
        if (WiFi.status() != WL_CONNECTED)
        {
            Serial.println("[WARN] Lost WiFi! Attempting to reconnect...");
            int retryCount = 0;

            while (WiFi.status() != WL_CONNECTED && retryCount < 20)
            {
                Serial.printf("Reconnect attempt %d: WiFi Status = %d\n", retryCount + 1, WiFi.status());
                resetWatchdog();
                vTaskDelay(pdMS_TO_TICKS(1000));
                retryCount++;
            }

            if (WiFi.status() == WL_CONNECTED)
            {
                Serial.println("[INFO] WiFi Reconnected!");
            }
            else
            {
                Serial.println("[ERROR] WiFi reconnect failed. Restarting in 5s...");
                vTaskDelay(pdMS_TO_TICKS(5000));
                ESP.restart();
            }
        }

        resetWatchdog();
        vTaskDelay(pdMS_TO_TICKS(5000));
    }
}

void wifi_init()
{
    xTaskCreate(taskWifi, "TaskWifi", 4096, NULL, 2, NULL);
}
