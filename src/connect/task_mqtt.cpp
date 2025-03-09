#include "task_mqtt.h"

String user = MQTT_USERS;
String password = MQTT_PASSWORD;

WiFiClient espClient;
PubSubClient client(espClient);
void callback(char *topic, byte *payload, unsigned int length)
{
    Serial.print("[MQTT] Message arrived: ");
    Serial.println(topic);
    String message;
    for (unsigned int i = 0; i < length; i++)
    {
        message += (char)payload[i];
    }

    Serial.print("[MQTT] Payload: ");
    Serial.println(message);

    if (String(topic).startsWith("v1/gateway/rpc"))
    {
        Serial.println("[MQTT] Data received from Gateway");
        // Parse JSON
        JsonDocument doc;
        DeserializationError error = deserializeJson(doc, message);

        if (error)
        {
            Serial.print("[ERROR] deserializeJson() failed: ");
            Serial.println(error.f_str());
            return;
        }
        String device = doc["device"].as<String>();
        // Lấy object "data"
        JsonObject data = doc["data"].as<JsonObject>();

        String method = data["method"].as<String>();
        String params = data["params"].as<String>();

        Serial.print("Method: ");
        Serial.println(method);
        Serial.print("Params: ");
        Serial.println(params);
        if (device == "test_3" || device == "test_2" || device == "test_4")
        {
            if (method == "setState")
            {
                Serial.print("Check for ");
                Serial.println(device);
                // Code for sending message to control relay with LoRa to node
                ////////////////
                // To do code

                //////////////
                JsonDocument jsonDoc;

                JsonArray deviceArray = jsonDoc[device].to<JsonArray>();
                JsonObject statusObj = deviceArray.add<JsonObject>();
                statusObj["switchstate"] = params;

                char buffer[512];
                serializeJson(jsonDoc, buffer, sizeof(buffer));

                publishData(MQTT_SENDING_VALUE, buffer);

                Serial.println("Sending Data to Gateway:");
                Serial.println(buffer);
            }
            if (method == "setPWM")
            {
                Serial.print("Check for ");
                Serial.println(device);

                // Code for sending messag to adjust pwm value with LoRa to node

                ///////////////
                // To do code

                //////////////
                JsonDocument jsonDoc;

                JsonArray deviceArray = jsonDoc[device].to<JsonArray>();
                JsonObject statusObj = deviceArray.add<JsonObject>();
                statusObj["pwm_value"] = params;

                char buffer[512];
                serializeJson(jsonDoc, buffer, sizeof(buffer));

                publishData(MQTT_SENDING_VALUE, buffer);

                Serial.println("Sending Data to Gateway:");
                Serial.println(buffer);
            }
        }
    }
}

bool publishData(String feedName, String message)
{
    String topic = feedName;
#ifdef ADAFRUIT
    String topic = user + "/feeds/" + feedName;
#endif
    printData(MQTT_FEED_NOTHING, "Publishing to topic: ");
    printData(MQTT_FEED_NOTHING, feedName + " ");
    printData(MQTT_FEED_NOTHING, "Status: ");

    if (client.publish(topic.c_str(), message.c_str(), 1))
    {
        printlnData(MQTT_FEED_NOTHING, "Success!: " + message);
        return 1;
    }
    printlnData(MQTT_FEED_NOTHING, "Failed!: " + message);
    return 0;
}

void reconnectMQTT()
{
    while (!client.connected())
    {
        printlnData(MQTT_FEED_NOTHING, "Connecting to MQTT...");

        // String clientId = "ESP32Client" + String(random(0, 1000));
        String clientId = "pole_1";
        // if (client.connect(clientId.c_str(), user.c_str(), password.c_str()))
        if (client.connect(clientId.c_str(), TOKEN_GATEWAY, ""))
        {
            printlnData(MQTT_FEED_NOTHING, "MQTT Connected");

// Subscribe to topic put in here
#ifdef _ESP_NUMBER_ONE_
            client.subscribe(MQTT_REQUEST_TOPIC);
            Serial.println("Successfully subscribe topic");
#endif

            printlnData(MQTT_FEED_NOTHING, "Start");
        }
        else
        {
            printData(MQTT_FEED_NOTHING, "MQTT connection failed, rc=");
            printlnData(MQTT_FEED_NOTHING, String(client.state()));
        }
        vTaskDelay(5000 / portTICK_PERIOD_MS);
    }
}

void taskMQTT(void *pvParameters)
{
    while (WiFi.status() != WL_CONNECTED)
    {
        vTaskDelay(delay_connect / portTICK_PERIOD_MS);
    }

    client.setServer(MQTT_SERVER, MQTT_PORT);
    client.setKeepAlive(30);
    client.setCallback(callback);
    Serial.println("check point mqtt");
#ifdef _ESP_NUMBER_ONE_
    client.subscribe(MQTT_REQUEST_TOPIC);
#endif

    while (true)
    {
        if (!client.connected())
        {
            reconnectMQTT();
        }

        client.loop();
        vTaskDelay(delay_mqtt / portTICK_PERIOD_MS);
    }
}

void mqtt_init()
{
    xTaskCreate(taskMQTT, "TaskMQTT", 4096, NULL, 1, NULL);
}