#include "task_wifi.h"

void taskWifi(void *pvParameters)
{
    Serial.println("check point");
    addTaskToWatchdog(NULL);
    Serial.println("check point 2");

    WiFi.mode(WIFI_STA);

    printData("Connecting to SSID: ");
    printlnData(WIFI_SSID);

    WiFi.begin(WIFI_SSID, WIFI_PASS);

    int wifiRetryCount = 0;
    while (WiFi.status() != WL_CONNECTED && wifiRetryCount < 20)
    {
        if (nullptr != getDebugSerial())
        {
            getDebugSerial()->printf("Attempt %d: WiFi Status = %d\n", wifiRetryCount + 1, WiFi.status());
        }
        resetWatchdog();
        vTaskDelay(pdMS_TO_TICKS(1000));
        wifiRetryCount++;
    }

    if (WiFi.status() != WL_CONNECTED)
    {
        printlnData("[ERROR] WiFi connection failed! Restarting in 5s...");
        vTaskDelay(pdMS_TO_TICKS(5000));
        ESP.restart();
    }

    printlnData("[INFO] WiFi Connected!");
    printData("IP Address: ");
    printlnData(String(WiFi.localIP()));

    while (true)
    {
        if (WiFi.status() != WL_CONNECTED)
        {
            printlnData("[WARN] Lost WiFi! Attempting to reconnect...");
            int retryCount = 0;

            while (WiFi.status() != WL_CONNECTED && retryCount < 20)
            {
                if (nullptr != getDebugSerial())
                {
                    getDebugSerial()->printf("Reconnect attempt %d: WiFi Status = %d\n", retryCount + 1, WiFi.status());
                }
                resetWatchdog();
                vTaskDelay(pdMS_TO_TICKS(1000));
                retryCount++;
            }

            if (WiFi.status() == WL_CONNECTED)
            {
                printlnData("[INFO] WiFi Reconnected!");
            }
            else
            {
                printlnData("[ERROR] WiFi reconnect failed. Restarting in 5s...");
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
