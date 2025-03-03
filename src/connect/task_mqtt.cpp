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

    if (String(topic).startsWith("v1/devices/me/rpc/request/"))
    {
        Serial.println("[MQTT] Data received from POLE_02");

        // Parse JSON
        JsonDocument doc;
        DeserializationError error = deserializeJson(doc, message);

        if (error)
        {
            Serial.print("[ERROR] deserializeJson() failed: ");
            Serial.println(error.f_str());
            return;
        }

        String method = doc["method"].as<String>();
        String params = doc["params"].as<String>();

        Serial.print("[MQTT] Method: ");
        Serial.println(method);
        Serial.print("[MQTT] Params: ");
        Serial.println(params);

        if (method == "getRelayState")
        {

            JsonDocument doc;
            doc["switchState"] = "high";
            char buffer[256];
            serializeJson(doc, buffer);
            publishData(MQTT_SENDING_VALUE, buffer);
        }
        if (method == "setState" && params == "high")
        {
            Serial.println("[MQTT] LED TURNED ON");
            setRelayOn();
            JsonDocument relay_obj;
            relay_obj["switchstate"] = "high";
            char buffer[256];
            serializeJson(relay_obj, buffer);
            publishData(MQTT_SENDING_VALUE, buffer);
        }
        else if (method == "setState" && params == "low")
        {
            Serial.println("[MQTT] LED TURNED OFF");
            setRelayOff();
            JsonDocument relay_obj;
            relay_obj["switchstate"] = "low";
            char buffer[256];
            serializeJson(relay_obj, buffer);
            publishData(MQTT_SENDING_VALUE, buffer);
        }
        if (method == "setPWM")
        {
            int pwm_value = params.toInt();
            pwm_set_duty(pwm_value);
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
        if (client.connect(clientId.c_str(), user.c_str(), password.c_str()))
        {
            printlnData(MQTT_FEED_NOTHING, "MQTT Connected");

// Subscribe to topic put in here
#ifdef _ESP_NUMBER_ONE_
            client.subscribe(MQTT_FEED_POLE_02);
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
    client.subscribe(MQTT_FEED_POLE_02);
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