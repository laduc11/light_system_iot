#include "serializer.h"

enum SerializerStatus
{
  SERIALIZER_OK = 0,
  SERIALIZER_ERROR = -1
};

/*===========================================Private functions===========================================*/

/*>>----------------------------------------Deserialize function-----------------------------------------<<*/
SerializerStatus deserializeControlRelayMessage(LoraMessage &out_msg, const String &dataraw)
{
  /**
   * Example control relay message format:
   * {
   *      "Address": "0001",
   *      "data": {
   *          "Relay": "high" | "low" | "toggle"
   *      }
   * }
   */

  // Deserialize the control relay message
  JsonDocument doc;
  DeserializationError error = deserializeJson(doc, dataraw.c_str() + 1); // Exclude the first byte (message ID)
  if (error)
  {
    printData("[ERROR] deserializeJson() failed: ");
    printlnData(error.f_str());
    return SERIALIZER_ERROR;
  }

  // Get value from Json message
  ControlRelayMessage *control_relay_message = new ControlRelayMessage();
  uint16_t source_address = (uint16_t)strtol(doc["Address"].as<String>().c_str(), nullptr, 16);
  JsonObject data = doc["data"].as<JsonObject>();
  String state = data["Relay"].as<String>();

  // Pass value to ControlRelayMessage's attributes
  control_relay_message->source_address = source_address;
  if (state == "high")
  {
    control_relay_message->relay_state = RELAY_HIGH;
  }
  else if (state == "low")
  {
    control_relay_message->relay_state = RELAY_LOW;
  }
  else if (state == "toggle")
  {
    control_relay_message->relay_state = RELAY_TOGGLE;
  }
  else
  {
    control_relay_message->relay_state = RELAY_UNKNOWN;
  }

  // Fill the LoraMessage object
  if (out_msg.data != nullptr)
  {
    printlnData("[WARNING] Free previous data in LoraMessage.");
    free(out_msg.data);
  }
  out_msg.data = (void *)control_relay_message;
  out_msg.type = LORA_MESSAGE_CONTROL_RELAY;

  return SERIALIZER_OK;
}

SerializerStatus deserializeControlBrightnessMessage(LoraMessage &out_msg, const String &dataraw)
{
  /**
   * Example control brightness message format:
   * {
   *      "Address": "0001",
   *      "data": {
   *          "PWM": "50" // 0-100
   *      }
   * }
   */

  // Deserialize the control brightness message
  JsonDocument doc;
  DeserializationError error = deserializeJson(doc, dataraw.c_str() + 1); // Exclude the first byte (message ID)
  if (error)
  {
    printData("[ERROR] deserializeJson() failed: ");
    printlnData(error.f_str());
    return SERIALIZER_ERROR;
  }

  // Get value from Json message
  ControlBrightnessMessage *control_brightness_message = new ControlBrightnessMessage();
  uint16_t source_address = (uint16_t)strtol(doc["Address"].as<String>().c_str(), nullptr, 16);
  JsonObject data = doc["data"].as<JsonObject>();
  int8_t brightness = data["PWM"].as<String>().toInt();
  if (brightness < 0 || brightness > 100)
  {
    printlnData("[WARNING] brightness value is invalid");
    printlnData("Set brightness to 0.");
    brightness = 0;
  }

  // Pass value to ControlRelayMessage's attributes
  control_brightness_message->source_address = source_address;
  control_brightness_message->brightness = brightness;

  // Fill the LoraMessage object
  if (out_msg.data != nullptr)
  {
    printlnData("[WARNING] Free previous data in LoraMessage.");
    free(out_msg.data);
  }
  out_msg.data = (void *)control_brightness_message;
  out_msg.type = LORA_MESSAGE_CONTROL_BRIGHTNESS;

  return SERIALIZER_OK;
}

SerializerStatus deserializeScanMessage(LoraMessage &out_msg, const String &dataraw)
{
  // Deserialize the scan message
  JsonDocument doc;
  DeserializationError error = deserializeJson(doc, dataraw.c_str() + 1); // Exclude the first byte (message ID)
  if (error)
  {
    printData("[ERROR] deserializeJson() failed: ");
    printlnData(error.f_str());
    return SERIALIZER_ERROR;
  }

  // Get value from Json message
  ScanMessage *scan_message = new ScanMessage();
  uint16_t source_address = (uint16_t)strtol(doc["Address"].as<String>().c_str(), nullptr, 16);
  scan_message->source_address = source_address;

  // Fill the LoraMessage object
  if (out_msg.data != nullptr)
  {
    printlnData("[WARNING] Free previous data in LoraMessage.");
    free(out_msg.data);
  }
  out_msg.data = (void *)scan_message;
  out_msg.type = LORA_MESSAGE_SCAN;
  return SERIALIZER_OK;
}

SerializerStatus deserializeScanResponseMessage(LoraMessage &out_msg, const String &dataraw)
{
  // Deserialize the scan response message
  JsonDocument doc;
  DeserializationError error = deserializeJson(doc, dataraw.c_str() + 1); // Exclude the first byte (message ID)
  if (error)
  {
    printData("[ERROR] deserializeJson() failed: ");
    printlnData(error.f_str());
    return SERIALIZER_ERROR;
  }

  // Get value from Json message
  ScanMessage *scan_response_message = new ScanMessage();
  uint16_t source_address = (uint16_t)strtol(doc["Address"].as<String>().c_str(), nullptr, 16);
  scan_response_message->source_address = source_address;

  // Fill the LoraMessage object
  if (out_msg.data != nullptr)
  {
    printlnData("[WARNING] Free previous data in LoraMessage.");
    free(out_msg.data);
  }
  out_msg.data = (void *)scan_response_message;
  out_msg.type = LORA_MESSAGE_SCAN_RESPONSE;
  return SERIALIZER_OK;
}

/*>>----------------------------------------serialize function-----------------------------------------<<*/
void serializeControlRelayMessage(SerializedMessage &out_msg, LoraMessage lora_message)
{
  // Cast data in lora message to ControlRelayMessage
  ControlRelayMessage *message = (ControlRelayMessage *)lora_message.data;
  // Create message with format
  /*
  bytes[0]: Message type
  bytes[1...]: Message data -> Using json library
  Json format:
  {
    Address: "0001",    // Destination address
    data: {
      Relay: "high" | "low" | "toggle"
    }
  }
  */
  JsonDocument doc;
  doc["Address"] = String(message->destination_address, HEX);
  if (message->relay_state == RELAY_HIGH)
  {
    doc["data"]["Relay"] = "high";
  }
  else if (message->relay_state == RELAY_LOW)
  {
    doc["data"]["Relay"] = "low";
  }
  else if (message->relay_state == RELAY_TOGGLE)
  {
    doc["data"]["Relay"] = "toggle";
  }

  // Serialize JSON directly into the out_msg.data buffer
  size_t jsonSize = measureJson(doc);
  out_msg.data = new uint8_t[jsonSize + 2]; // Need 1 yte for type message, 1 byte for null-terminate
  out_msg.data[0] = static_cast<uint8_t>(LORA_MESSAGE_CONTROL_RELAY);
  serializeJson(doc, reinterpret_cast<char *>(out_msg.data + 1), jsonSize + 1);
  out_msg.data[jsonSize + 1] = '\0';
  out_msg.length = jsonSize + 2;
}

void serializeControlBrightnessMessage(SerializedMessage &out_msg, LoraMessage lora_message)
{
  // Cast data in lora message to ControlRelayMessage
  ControlBrightnessMessage *message = (ControlBrightnessMessage *)lora_message.data;
  // Create message with format
  /*
  bytes[0]: Message type
  bytes[1...]: Message data -> Using json library
  Json format:
  {
    Address: "0001",    // Destination address
    data: {
      PWM: "50" // 0-100
    }
  }
  */
  JsonDocument doc;
  doc["Address"] = String(message->destination_address, HEX);
  doc["data"]["PWM"] = String(message->brightness);

  // Serialize JSON directly into the out_msg.data buffer
  size_t jsonSize = measureJson(doc);
  out_msg.data = new uint8_t[jsonSize + 2]; // Need 1 yte for type message, 1 byte for null-terminate
  out_msg.data[0] = static_cast<uint8_t>(LORA_MESSAGE_CONTROL_BRIGHTNESS);
  serializeJson(doc, reinterpret_cast<char *>(out_msg.data + 1), jsonSize + 1);
  out_msg.data[jsonSize + 1] = '\0';
  out_msg.length = jsonSize + 2;
}

void serializeScanMessage(SerializedMessage &out_msg, LoraMessage lora_message)
{
  // Cast data in lora message to ScanMessage
  ScanMessage *message = (ScanMessage *)lora_message.data;
  // Create message with format
  /*
  bytes[0]: Message type
  bytes[1...]: Message data -> Using json library
  Json format:
  {
    Address: "0001",    // Source address
  }
  */
  JsonDocument doc;
  doc["Address"] = String(message->source_address, HEX);

  // Serialize JSON directly into the out_msg.data buffer
  size_t jsonSize = measureJson(doc);
  out_msg.data = new uint8_t[jsonSize + 2]; // Need 1 yte for type message, 1 byte for null-terminate
  out_msg.data[0] = static_cast<uint8_t>(LORA_MESSAGE_SCAN);
  serializeJson(doc, reinterpret_cast<char *>(out_msg.data + 1), jsonSize + 1);
  out_msg.data[jsonSize + 1] = '\0';
  out_msg.length = jsonSize + 2;
}

void serializeScanResponseMessage(SerializedMessage &out_msg, LoraMessage lora_message)
{
  // Cast data in lora message to ScanMessage
  ScanMessage *message = (ScanMessage *)lora_message.data;
  // Create message with format
  /*
  bytes[0]: Message type
  bytes[1...]: Message data -> Using json library
  Json format:
  {
    Address: "0001",    // Source address
    data: [
      {
        Address: "0001",    // Node address
        RSSI: "-50"        // RSSI value
      },
      {
        Address: "0002",
        RSSI: "-60"
      },
      {
        Address: "0003",
        RSSI: "-70"
      }
    ]
  }
  */
  JsonDocument doc;
  doc["Address"] = String(message->source_address, HEX);

  // Serialize JSON directly into the out_msg.data buffer
  size_t jsonSize = measureJson(doc);
  out_msg.data = new uint8_t[jsonSize + 2]; // Need 1 yte for type message, 1 byte for null-terminate
  out_msg.data[0] = static_cast<uint8_t>(LORA_MESSAGE_SCAN_RESPONSE);
  serializeJson(doc, reinterpret_cast<char *>(out_msg.data + 1), jsonSize + 1);
  out_msg.data[jsonSize + 1] = '\0';
  out_msg.length = jsonSize + 2;
}

/*=======================================================================================================*/

/*===========================================Public functions===========================================*/

void deserializeLoraMessage(LoraMessage &out_msg, const String &dataraw)
{
  // Check if the input string is empty
  if (dataraw.length() == 0)
  {
    printData("[ERROR] deserializeLoraMessage() failed: ");
    printlnData("dataraw is empty.");
    return;
  }

  // Check the message ID (First byte is message ID)
  if ((uint8_t)dataraw[0] > LORA_MESSAGE_UNKNOWN)
  {
    printlnData("[ERROR] deserializeLoraMessage() failed: ");
    printlnData("Invalid message ID.");
    return;
  }

  // Parse the message type and data
  LoraMessageType message_type = static_cast<LoraMessageType>((uint8_t)dataraw[0]);

  // Process the message control relay
  if (message_type == LORA_MESSAGE_CONTROL_RELAY)
  {
    deserializeControlRelayMessage(out_msg, dataraw);
  }
  else if (message_type == LORA_MESSAGE_CONTROL_BRIGHTNESS)
  {
    deserializeControlBrightnessMessage(out_msg, dataraw);
  }
  else if (message_type == LORA_MESSAGE_SCAN)
  {
    deserializeScanMessage(out_msg, dataraw);
  }
  else if (message_type == LORA_MESSAGE_SCAN_RESPONSE)
  {
    deserializeScanResponseMessage(out_msg, dataraw);
  }
  else
  {
    printlnData("[ERROR] deserializeLoraMessage() failed: ");
    printlnData("Unknown message type.");
    return;
  }
}

void serializeLoraMessage(SerializedMessage &out_msg, LoraMessage lora_message)
{
  LoraMessageType type = lora_message.type;

  if (type == LORA_MESSAGE_CONTROL_RELAY)
  {
    serializeControlRelayMessage(out_msg, lora_message);
  }
  else if (type == LORA_MESSAGE_CONTROL_BRIGHTNESS)
  {
    serializeControlBrightnessMessage(out_msg, lora_message);
  }
  else if (type == LORA_MESSAGE_SCAN)
  {
    serializeScanMessage(out_msg, lora_message);
  }
  else if (type == LORA_MESSAGE_SCAN_RESPONSE)
  {
    serializeScanResponseMessage(out_msg, lora_message);
  }
  else
  {
    printlnData("[ERROR] serializeLoraMessage() failed: ");
    printlnData("Unknown message type.");
    return;
  }
}