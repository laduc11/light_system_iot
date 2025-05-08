#ifndef SERIALIZER_H
#define SERIALIZER_H

#include <objects.h>
#include <ArduinoJson.h>
#include <utils/print_debug.h>

/*
* Using Json is a quick way at this time, but it is not the best way to serialize data.
* We should use a better way to serialize data in the future.
*/

/**
 * @brief Deserializes a JSON string to object LoraMessage.
 * 
 * @param out_msg The object to be deserialized.
 * @param dataraw The string to be deserialized. 
 * (Can be a JSON string or literal string)
 */
void deserializeLoraMessage(LoraMessage &out_msg, const String &dataraw);

/**
 * @brief Serializes a LoraMessage object to array of bytes.
 * 
 * @param out_msg The object to be serialized.
 * @param msg The object to be serialized.
 */
void serializeLoraMessage(SerializedMessage &out_msg, LoraMessage lora_message);

#endif // SERIALIZER_H