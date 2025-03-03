#include "task_wifi.h"

void taskWifi(void *pvParameters)
{
    addTaskToWatchdog(NULL);
    WiFi.mode(WIFI_STA);
    String ssid = WIFI_SSID;
    String password = WIFI_PASS;

    Serial.print("Connecting to SSID: ");
    Serial.println(WIFI_SSID);
 
    WiFi.begin(ssid.c_str(), password.c_str());

    int wifiRetryCount = 0;
    while (WiFi.status() != WL_CONNECTED && wifiRetryCount < 20)
    {
        printlnData(MQTT_FEED_NOTHING, "Connecting to WiFi...");
        resetWatchdog();
        Serial.print(".");
        vTaskDelay(1000 / portTICK_PERIOD_MS);
        wifiRetryCount++;
    }

    if (WiFi.status() != WL_CONNECTED)
    {
        Serial.println("\nWiFi connection failed! Restarting...");
        ESP.restart();
    }

    printlnData(MQTT_FEED_NOTHING, "Connected to WiFi");
    Serial.println("\nWiFi Connected!");
    Serial.print("IP Address: ");
    Serial.println(WiFi.localIP());

    while (true)
    {
        if (WiFi.status() != WL_CONNECTED)
        {
            printlnData(MQTT_FEED_NOTHING, "Lost WiFi, reconnecting...");
            WiFi.begin(ssid.c_str(), password.c_str());
            int retryCount = 0;
            while (WiFi.status() != WL_CONNECTED && retryCount < 20)
            {
                printlnData(MQTT_FEED_NOTHING, "Reconnecting to WiFi...");
                resetWatchdog();
                vTaskDelay(delay_wifi / portTICK_PERIOD_MS);
                retryCount++;
            }

            if (WiFi.status() == WL_CONNECTED)
            {
                printlnData(MQTT_FEED_NOTHING, "Reconnected to WiFi...");
            }
            else
            {
                printlnData(MQTT_FEED_NOTHING, "WiFi reconnect failed, restarting...");
                ESP.restart();
            }
        }
        resetWatchdog();
        vTaskDelay(delay_wifi / portTICK_PERIOD_MS);
    }
}

void wifi_init()
{
    xTaskCreate(taskWifi, "TaskWifi", 4096, NULL, 2, NULL);
}
