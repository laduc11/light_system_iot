#include "loraTaskCallback.h"

Pole::Pole(uint16_t address = 0, float temp = 0.0, float humi = 0.0, float intensity = 0.0)
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
String Pole::serializeJsonPKG()
{
    JsonDocument doc;
    doc["header"] = "period";
    JsonObject data = doc["Pole_Detail"].to<JsonObject>();
    data["address"] = this->address;
    data["temp"] = String(this->temp, 2);
    data["humidity"] = String(this->humi, 2);
    data["intensity"] = String(this->intensity, 2);

    String pkg;
    serializeJson(doc, pkg);
    return pkg;
}
void Pole::deserializeJsonPKG(String msg)
{
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

    // get value from Json message
    String address = doc["Address"].as<String>(); // _001 -> 1
    JsonObject data = doc["data"].as<JsonObject>();
    String state = data["Relay"].as<String>();
    String pwm_val = data["PWM"].as<String>();
    
    // Pass value to Node's attributes
    node.address = address.toInt();
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

void LoRaRecvTask(void *pvParameters) {
    BasicQueue<String> *q = (BasicQueue<String> *)pvParameters;
    RecvFrame_t data;
    String data_buffer;
    while (1)
    { 
      if (getLoraIns()->RecieveFrame(&data) == 0) 
      {
        data_buffer = String(data.recv_data, data.recv_data_len);
        Serial.print("Data size: ");
        Serial.println(data.recv_data_len);
        Serial.print(data_buffer);
          
        Serial.println();
        Serial.printf("RSSI: %d dBm\n", data.rssi);
        Serial.flush();
        if (data.rssi != -256 || data.rssi != -1) q->push_back(data_buffer);
      }
      vTaskDelay(pdMS_TO_TICKS(delay_rev_lora_process));
    }
  }

void controlRelay(String device, String state)
{
    if (!getLoraIns() || !getConfigLora())
    {
        Serial.println("LoRa is not initialized or config fail");
        return;
    }
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