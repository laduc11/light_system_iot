#include "loraTaskCallback.h"

BasicQueue<String> *buffer_G2N = new BasicQueue<String>;

BasicQueue<String> *get_bufferG2N()
{
    return buffer_G2N;
}

Pole::Pole(uint16_t address, float temp, float humi, float intensity)
{
    this->address = address;
    this->humi = humi;
    this->temp = temp;
    this->intensity = intensity;
}
void Pole::setPole(uint16_t address, float temp, float humi, float intensity)
{
    this->address = address;
    this->humi = humi;
    this->temp = temp;
    this->intensity = intensity;
}
void Pole::setPole(const JsonObject &body)
{
    this->address = body["Address"];
    this->temp = body["temp"];
    this->humi = body["humidity"];
    this->intensity = body["intensity"];
}
String Pole::serializeJsonPKG()
{
    // {"Header":"PERIOD_SS","Body":{"Address": "1","temp":"10.20","humidity;":"10.00","intensity":"20.00"}}
    JsonDocument doc;
    doc["Header"] = "PERIOD_SS";
    JsonObject data = doc["Body"].to<JsonObject>();
    data["Address"] = this->address;
    data["temp"] = String(this->temp, 2);
    data["humidity"] = String(this->humi, 2);
    data["intensity"] = String(this->intensity, 2);

    String pkg;
    serializeJson(doc, pkg);
    return pkg;
}
void Pole::deserializeJsonPKG(String msg)
{
    JsonDocument doc;
    DeserializationError error = deserializeJson(doc, msg);

    if (error)
    {
        printData("[ERROR] deserializeJson() failed: ");
        printlnData(error.f_str());
        return;
    }

    JsonObject body = doc["Body"].as<JsonObject>();
    this->setPole(body);
    return;
}

NodeStatus deserializeJsonFormat(const String &dataraw)
{
    NodeStatus node;
    JsonDocument doc;
    DeserializationError error = deserializeJson(doc, dataraw);

    if (error)
    {
        printData("[ERROR] deserializeJson() failed: ");
        printlnData(error.f_str());
        return node;
    }

    JsonObject body = doc["Body"].as<JsonObject>();
    // get value from Json message
    String address = body["Address"].as<String>(); // _001 -> 1
    String target = body["Target"].as<String>();
    JsonObject data = body["data"].as<JsonObject>();
    String state = data["Relay"].as<String>();
    String pwm_val = data["PWM"].as<String>();

    // Pass value to Node's attributes
    node.address = address.toInt();
    node.target = target.toInt();
    node.pwm_val = pwm_val.toInt();
    if (state == "high") // do server gui ve qui dinh high/low
        node.state = 1;
    if (state == "low")
        node.state = 0;
    if (state == "toggle")
        node.state = 10;
    Serial.println("Deserialize data success.");
    return node;
}

String serializeJsonFormat(String address, String target, String method, String value)
{
    // {"Header":"ACK_STT", "Body":{"Address": "1", "Target": "1", data":{"Relay":"unchanged", "PWM":"50"}}}
    JsonDocument doc;
    doc["Header"] = "ACK_STT";
    JsonObject body = doc["Body"].to<JsonObject>();
    body["Address"] = address; // source address
    body["Target"] = target;
    JsonObject data = body["data"].to<JsonObject>();
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
    else
        return "";
    String jsonStr;
    serializeJson(doc, jsonStr);

    return jsonStr;
}

void LoRaRecvTask(void *pvParameters)
{
    BasicQueue<String> *q = (BasicQueue<String> *)pvParameters;
    RecvFrame_t data;
    String data_buffer;
    while (1)
    {
        if (getLoraIns()->RecieveFrame(&data) == 0)
        {
            data_buffer = String(data.recv_data, data.recv_data_len);
            // Serial.print("Data size: ");
            // Serial.println(data.recv_data_len);
            // Serial.print(data_buffer);

            // Serial.println();
            Serial.printf("RSSI: %d dBm\n", data.rssi);
            // Serial.flush();
            if (data.rssi != -256 || data.rssi != -1)
                q->push_back(data_buffer);
        }
        vTaskDelay(pdMS_TO_TICKS(delay_rev_lora_process));
    }
}

void controlRelay(String device, String state, String &message)
{
    if (!getLoraIns() || !getConfigLora())
    {
        Serial.println("LoRa is not initialized or config fail");
        return;
    }
    String target = device.substring(device.indexOf(' '));
    String address = String(GATEWAY_ADDR);
    message = serializeJsonFormat(address, target, "Relay", state);

    // Push message to Sending buffer (buffer gateway to Node)
    buffer_G2N->push_back(message);
    // LoRa_E220_JP *lora_ptr = getLoraIns();

    // if (lora_ptr->SendFrame(*getConfigLora(), (uint8_t *)message.c_str(), message.length()) == 0)
    // {

    //     Serial.print("Send message control relay to device: ");
    //     Serial.print(device);
    //     Serial.println(" success");
    // }
    // else
    // {
    //     Serial.println("Send message failed.");
    // }
    Serial.flush();
}

void controlPwm(String device, String value, String &message)
{
    if (!getLoraIns() || !getConfigLora())
    {
        Serial.println("LoRa is not initialized or config fail");
        return;
    }
    // msg = device + " { " + pwm_template + value + " }"; // SmartPole 001 { PWM: 50 }
    String target = device.substring(device.indexOf(' '));
    String address = String(GATEWAY_ADDR);
    message = serializeJsonFormat(address, target, "PWM", value);

    // Push message to Sending buffer (buffer gateway to Node)
    buffer_G2N->push_back(message);
    // Send message via LoRa
    // LoRa_E220_JP *lora_ptr = getLoraIns();
    // if (lora_ptr->SendFrame(*getConfigLora(), (uint8_t *)message.c_str(), message.length()) == 0)
    // {

    //     Serial.print("Send message control Pwm to device: ");
    //     Serial.print(device);
    //     Serial.println(" success");
    // }
    // else
    // {
    //     Serial.println("Send message failed.");
    // }
    // Serial.flush();
}

void sendLora(const String &msg)
{
    if (getLoraIns()->SendFrame(*(getConfigLora()), (uint8_t *)msg.c_str(), msg.length()) == 0)
    {
        printData("Send SUCCESS message via LORA to addr 2, data: ");
        printlnData(msg);
    }
    else
    {
        printlnData("Send message FAIL via LORA, data: ");
        printlnData(msg);
    }
    Serial.flush();
}