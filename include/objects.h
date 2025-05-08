#ifndef OBJECTS_H
#define OBJECTS_H

#include <Arduino.h>

class NodeStatus
{
public:
    uint16_t address;
    int8_t state;
    int8_t pwm_val;

    /**
     * @brief Construct a new Node Status object
     * 
     * @param add Node's address
     * @param state Light state (0: low, 1: high)
     * @param val Value of dimming control (0-100) if value is -1, it means unchanged
     */
    NodeStatus(uint16_t add = -1, int8_t state = 0, int8_t val = -1)
    {
        this->address = add;
        this->state = state;
        this->pwm_val = val;
    }
};

/**
 * @brief General message using in LoRa network. This message
 * contains source and destination address.
 * 
 */
class MessageHeader
{
public:
    uint16_t source_address;
    uint16_t destination_address;

    MessageHeader(uint16_t src = -1, uint16_t dest = -1)
    {
        this->source_address = src;
        this->destination_address = dest;
    }
};

/*===========Control message===========*/
class ControlRelayMessage : public MessageHeader
{
public:
    uint8_t relay_state; // 0: low, 1: high

    ControlRelayMessage(
        uint16_t src = -1, 
        uint16_t dest = -1, 
        uint8_t state = 0) 
        : MessageHeader(src, dest)
    {
        this->relay_state = state;
    }
};

class ControlBrightnessMessage : public MessageHeader
{
public:
    uint8_t brightness; // 0-100

    ControlBrightnessMessage(
        uint16_t src = -1, 
        uint16_t dest = -1, 
        uint8_t brightness = 0) 
        : MessageHeader(src, dest)
    {
        this->brightness = brightness;
    }
};

/*========================Scanning message========================*/
class ScanMessage : public MessageHeader
{
public:
    uint8_t scan_type; // 0: scan, 1: scan response

    ScanMessage(
        uint16_t src = -1, 
        uint16_t dest = -1, 
        uint8_t type = 0) 
        : MessageHeader(src, dest)
    {
        this->scan_type = type;
    }
};

/*========================General LoRa Message========================*/
enum LoraMessageType
{
    LORA_MESSAGE_CONTROL_RELAY,
    LORA_MESSAGE_CONTROL_BRIGHTNESS,
    LORA_MESSAGE_SCAN,
    LORA_MESSAGE_SCAN_RESPONSE,
    LORA_MESSAGE_UNKNOWN
};

enum RalayState
{
    RELAY_LOW,
    RELAY_HIGH,
    RELAY_TOGGLE,
    RELAY_UNKNOWN
};

class LoraMessage
{
public:
    LoraMessageType type;
    void *data; // Pointer to the data

    LoraMessage(
        LoraMessageType type = LORA_MESSAGE_UNKNOWN, 
        void *data = nullptr)
    {
        this->type = type;
        this->data = data;
    }

    ~LoraMessage()
    {
    }
};

class SerializedMessage
{
public:
    uint8_t *data; // Pointer to the data
    size_t length; // Length of the data

    SerializedMessage(
        uint8_t *data = nullptr, 
        size_t length = 0)
    {
        this->data = data;
        this->length = length;
    }

    ~SerializedMessage()
    {
    }
};

#endif // OBJECTS_H