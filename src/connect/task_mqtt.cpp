#include "task_mqtt.h"

String user = MQTT_USERS;
String password = MQTT_PASSWORD;

WiFiClient espClient;
PubSubClient client(espClient);

/*<=================================Private Function=================================>*/
// convert to this string {"Address":"SmartPole 001","data":{"Relay":"high/low","PWM":"50"}}
String serializeJsonFormat(String address, String method, String value)
{
    JsonDocument doc;

    doc["Address"] = address;
    JsonObject data = doc["data"].to<JsonObject>();
    if (method == "Relay")
    {
        data[method] = value;
        data["PWM"] = "-1";
    }
    else if (method == "PWM")
    {
        data[method] = value;
        data["Relay"] = "unchanged";
    }
    else return "";
    String jsonStr;
    serializeJson(doc, jsonStr);

    return jsonStr;
}

void controlRelay(String device, String state)
{
    if (!getLoraIns() || !getConfigLora())
    {
        Serial.println("LoRa is not initialized or config fail");
        return;
    }
    // String relay_template = "Relay: ";
    // String msg = "";
    // msg = device + " { " + relay_template + state + " }"; // SmartPole 001 { Relay: high/low }
    String address = device.substring(device.indexOf(' '));
    String msg = serializeJsonFormat(address, "Relay", state);

    // Send message via LoRa
    LoRa_E220_JP *lora_ptr = getLoraIns();

    if (lora_ptr->SendFrame(*getConfigLora(), (uint8_t *)msg.c_str(), msg.length()) == 0)
    {

        Serial.print("Send message control relay to device: ");
        Serial.print(device);
        Serial.println(" success");
    }
    else
    {
        Serial.println("Send message failed.");
    }
    Serial.flush();
}

void controlPwm(String device, String value)
{
    if (!getLoraIns() || !getConfigLora())
    {
        Serial.println("LoRa is not initialized or config fail");
        return;
    }
    // String pwm_template = "PWM: ";
    // String msg = "";
    // msg = device + " { " + pwm_template + value + " }"; // SmartPole 001 { PWM: 50 }
    String address = device.substring(device.indexOf(' '));
    String msg = serializeJsonFormat(address, "PWM", value);

    // Send message via LoRa
    LoRa_E220_JP *lora_ptr = getLoraIns();
    if (lora_ptr->SendFrame(*getConfigLora(), (uint8_t *)msg.c_str(), msg.length()) == 0)
    {

        Serial.print("Send message control Pwm to device: ");
        Serial.print(device);
        Serial.println(" success");
    }
    else
    {
        Serial.println("Send message failed.");
    }
    Serial.flush();
}
/*<=================================Private Function=================================>*/

void callback(char *topic, byte *payload, unsigned int length)
{
    printData("[MQTT] Message arrived: ");
    printlnData(topic);
    String message;
    for (unsigned int i = 0; i < length; i++)
    {
        message += (char)payload[i];
    }

    printData("[MQTT] Payload: ");
    printlnData(message);

    if (String(topic).startsWith("v1/gateway/rpc"))
    {
        printlnData("[MQTT] Data received from Gateway");
        // Parse JSON
        JsonDocument doc;
        DeserializationError error = deserializeJson(doc, message);

        if (error)
        {
            printData("[ERROR] deserializeJson() failed: ");
            printlnData(error.f_str());
            return;
        }

        // Get value from package receivce from server
        String device = doc["device"].as<String>();
        JsonObject data = doc["data"].as<JsonObject>();
        String method = data["method"].as<String>();
        String params = data["params"].as<String>();

        printData("Method: ");
        printlnData(method);
        printData("Params: ");
        printlnData(params);
        if (device == "SmartPole 001" || device == "SmartPole 002")
        {
            if (method == "setState")
            {
                printData("Check for ");
                printlnData(device);
                // Code for sending message to control relay with LoRa to node
                controlRelay(device, params);

                // Publish message to server to synchronous state of device
                JsonDocument jsonDoc;

                JsonArray deviceArray = jsonDoc[device].to<JsonArray>();
                JsonObject statusObj = deviceArray.add<JsonObject>();
                statusObj["switchstate"] = params;

                char buffer[512];
                serializeJson(jsonDoc, buffer, sizeof(buffer));

                publishData(MQTT_SENDING_VALUE, buffer);

                printlnData("Updating Relay state for device");
                printlnData(buffer);
            }
            if (method == "setPWM")
            {
                printData("Check for ");
                printlnData(device);

                // Code for sending messag to adjust pwm value with LoRa to node
                controlPwm(device, params);
                // Publish message to server to synchronous state of device

                JsonDocument jsonDoc;

                JsonArray deviceArray = jsonDoc[device].to<JsonArray>();
                JsonObject statusObj = deviceArray.add<JsonObject>();
                statusObj["pwm_value"] = params;

                char buffer[512];
                serializeJson(jsonDoc, buffer, sizeof(buffer));

                publishData(MQTT_SENDING_VALUE, buffer);

                printlnData("Updating PWM value for device");
                printlnData(buffer);
            }
        }
    }
}

void reconnectMQTT()
{
    while (!client.connected())
    {
        printlnData("Connecting to MQTT...");

        // String clientId = "ESP32Client" + String(random(0, 1000));
        String clientId = "pole_1";
        // if (client.connect(clientId.c_str(), user.c_str(), password.c_str()))
        if (client.connect(clientId.c_str(), TOKEN_GATEWAY, ""))
        {
            printlnData("MQTT Connected");

// Subscribe to topic put in here
#ifdef _ESP_NUMBER_ONE_
            client.subscribe(MQTT_REQUEST_TOPIC);
            printlnData("Successfully subscribe topic");
#endif

            printlnData("Start");
        }
        else
        {
            printData("MQTT connection failed, rc=");
            printlnData(String(client.state()));
        }
        vTaskDelay(5000 / portTICK_PERIOD_MS);
    }
}

void mqtt_init()
{
    xTaskCreate(taskMQTT, "TaskMQTT", 4096, NULL, 1, NULL);
}

/*<=================================Public Function=================================>*/
bool publishData(String feedName, String message)
{
    String topic = feedName;
#ifdef ADAFRUIT
    String topic = user + "/feeds/" + feedName;
#endif
    printData("Publishing to topic: ");
    printData(feedName + " ");
    printData("Status: ");

    if (client.publish(topic.c_str(), message.c_str(), 1))
    {
        printlnData("Success!: " + message);
        return 1;
    }
    printlnData("Failed!: " + message);
    return 0;
}

void taskMQTT(void *pvParameters)
{
    // Wait connecting Wifi
    while (WiFi.status() != WL_CONNECTED)
    {
        vTaskDelay(delay_connect / portTICK_PERIOD_MS);
    }

    client.setServer(MQTT_SERVER, MQTT_PORT);
    client.setKeepAlive(30);
    client.setCallback(callback);
    printlnData("check point mqtt");
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
