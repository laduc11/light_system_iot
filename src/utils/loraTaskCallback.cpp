#include "loraTaskCallback.h"

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
    BasicQueue<RecvFrame_t> *q = (BasicQueue<RecvFrame_t> *)pvParameters;
    RecvFrame_t data;
    String data_buffer;
    while (1)
    { 
      if (getLoraIns()->RecieveFrame(&data) == 0) 
      {
        // data_buffer = String(data.recv_data, data.recv_data_len);
        // Serial.print("Data size: ");
        // Serial.println(data.recv_data_len);
        // Serial.print(data_buffer);
        // Serial.println();
          
        Serial.printf("RSSI: %d dBm\n", data.rssi);
        Serial.flush();
        q->push_back(data);
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