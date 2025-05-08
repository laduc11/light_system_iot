#include "globals.h"
#include "utils/serializer.h"

// Handle after receive lora package from gateway
void handleProcessBuffer(void *pvParameters)
{
  BasicQueue<String> *q = (BasicQueue<String> *)pvParameters;
  BasicQueue<uint16_t> parent_address_queue;

  while(1)
  {
    if (!q->isEmpty())
    {
      String data = q->pop();
      // Deserialize the data to LoraMessage
      LoraMessage lora_message;
      deserializeLoraMessage(lora_message, data);
      // Check the message type and process accordingly
      if (lora_message.type == LORA_MESSAGE_CONTROL_RELAY)
      {
        ControlRelayMessage *control_relay_message = (ControlRelayMessage *)lora_message.data;
        // Process the contLrol relay message
        Serial.printf("Control Relay: Address: %04X, State: %d\n", control_relay_message->source_address, control_relay_message->relay_state);

        /* Send the same messagae back to the sender */
        SerializedMessage out_msg;
        // Update the source address and destination address for lora message
        control_relay_message->destination_address = control_relay_message->source_address;
        control_relay_message->source_address = getConfigLora()->own_address;
        // Serialize the message to send back
        serializeLoraMessage(out_msg, lora_message);
        // Send the message back to the sender
        getConfigLora()->target_address = control_relay_message->destination_address;
        getLoraIns()->SendFrame(*(getConfigLora()), out_msg.data, out_msg.length);
        printlnData("Send control relay message back");
        getConfigLora()->target_address = 0xffff;   // Reset target address to 0xffff
      }
      else if (lora_message.type == LORA_MESSAGE_CONTROL_BRIGHTNESS)
      {
        ControlBrightnessMessage *control_brightness_message = (ControlBrightnessMessage *)lora_message.data;
        // Process the control brightness message
        Serial.printf("Control Brightness: Address: %04X, Brightness: %d\n", control_brightness_message->source_address, control_brightness_message->brightness);
        
        /* Send the same messagae back to the sender */
        SerializedMessage out_msg;
        // Update the source address and destination address for lora message
        control_brightness_message->destination_address = control_brightness_message->source_address;
        control_brightness_message->source_address = getConfigLora()->own_address;
        // Serialize the message to send back
        serializeLoraMessage(out_msg, lora_message);
        // Send the message back to the sender
        getConfigLora()->target_address = control_brightness_message->destination_address;
        getLoraIns()->SendFrame(*(getConfigLora()), out_msg.data, out_msg.length);
        Serial.printf("Send control brightness message back");
        getConfigLora()->target_address = 0xffff;   // Reset target address to 0xffff
      }
      else if (lora_message.type == LORA_MESSAGE_SCAN)
      {
        // Handle scan message
        ScanMessage *scan_message = (ScanMessage *)lora_message.data;
        Serial.printf("Scan message: Address: %04X\n", scan_message->source_address);
        if (!parent_address_queue.find(scan_message->source_address))
        {
          parent_address_queue.push_back(scan_message->source_address);   // Add the address to the queue
        }
        else
        {
          Serial.printf("Address %04X already in the queue\n", scan_message->source_address);
          continue;
        }

        // Send scan response message back to the sender
        LoraMessage lora_message_response;
        lora_message_response.type = LORA_MESSAGE_SCAN_RESPONSE;
        ScanMessage *scan_response_message = new ScanMessage(getConfigLora()->own_address, scan_message->source_address, 1);
        scan_response_message->scan_type = 1;   // Scan response
        lora_message_response.data = (void *)scan_response_message;
        SerializedMessage out_msg;
        serializeLoraMessage(out_msg, lora_message_response);
        getConfigLora()->target_address = scan_response_message->destination_address;
        getLoraIns()->SendFrame(*(getConfigLora()), out_msg.data, out_msg.length);
        printlnData("Send scan response message back");
        getConfigLora()->target_address = 0xffff;   // Reset target address to 0xffff

        delete[] out_msg.data;   // Free the memory allocated for out_msg.data
        delete scan_response_message;   // Free the memory allocated for scan_response_message
      }
      else if (lora_message.type == LORA_MESSAGE_SCAN_RESPONSE)
      {
        // Handle scan response message
        // Get node address from message and print it
        ScanMessage *scan_response_message = (ScanMessage *)lora_message.data;
        Serial.printf("Scan response message: Address: %04X\n", scan_response_message->source_address);
      }
    }
    else
    {
      // Buffer is empty
#if 0
      LoraMessage lora_message;
      lora_message.type = LORA_MESSAGE_CONTROL_RELAY;
      ControlRelayMessage *control_relay_message = new ControlRelayMessage;
      control_relay_message->source_address = getConfigLora()->own_address;
      control_relay_message->destination_address = 0xffff;
      control_relay_message->relay_state = RELAY_HIGH;
      lora_message.data = (void *)control_relay_message;
      SerializedMessage out_msg;
      serializeLoraMessage(out_msg, lora_message);

      // Send the message back to the sender
      getLoraIns()->SendFrame(*(getConfigLora()), out_msg.data, out_msg.length);
      printlnData("Send control relay message");
      
      delete control_relay_message;   // Free the memory allocated for control_relay_message
#endif
      vTaskDelay(pdMS_TO_TICKS(1000));   // Delay 100ms to avoid busy waiting
    }
  }
  vTaskDelete(nullptr);
}

/**
 * @brief Send LoRa messge (RTOS task)
 * 
 * @param pvParameter pointer to sending buffer
 */
void sendLoraMessageTask(void *pvParameter)
{
  BasicQueue<String> *send_queue = (BasicQueue<String> *)pvParameter;
  LoRa_E220_JP *lora_ins = getLoraIns();
  LoRaConfigItem_t *config_param = getConfigLora();

  while (true) {
    if (!send_queue->isEmpty()) {
      String data = send_queue->pop();
      lora_ins->SendFrame(*config_param, (uint8_t *)data.c_str(), data.length());
      printlnData("Send message via LoRa success");
    } else {
      vTaskDelay(500);  // Delay 500 ms to avoid
    }
  }
}

/**
 * @brief Scan mechanism 
 * 
 * @param pvParameter send queue
 */
void scanNode(void *pvParameter)
{
  BasicQueue<String> *send_queue = (BasicQueue<String> *)pvParameter;
  LoRa_E220_JP *lora_ins = getLoraIns();
  LoRaConfigItem_t *config_param = getConfigLora();

  // Send the message to turn on scan mode in other mode
  LoraMessage lora_message;
  lora_message.type = LORA_MESSAGE_SCAN;
  ScanMessage *scan_message = new ScanMessage(config_param->own_address, config_param->target_address);
  lora_message.data = (void *)scan_message;
  SerializedMessage serialized_message;
  serializeLoraMessage(serialized_message, lora_message);

  // Scan node with retry 3 times 
  for (int i = 0; i < 3; i++)
  {
    send_queue->push_back(String(serialized_message.data, serialized_message.length));
    vTaskDelay(pdMS_TO_TICKS(5000));   // Delay 5 second
  }
}

/**
 * @brief Produce message to send via LoRa (use for debugging)
 * 
 * @param pvParameter sending buffer
 */
void produceMessage(void *pvParameter) 
{
  BasicQueue<String> *send_buffer = (BasicQueue<String> *)pvParameter;
  LoRaConfigItem_t *config_param = getConfigLora();

  // Create control relay message every 1 second
  while (true)
  {
    // Create control relay message
    ControlRelayMessage *message = new ControlRelayMessage(
      config_param->own_address,
      config_param->target_address,
      static_cast<uint8_t>(RELAY_TOGGLE)
    );

    LoraMessage lora_message;
    lora_message.type = LORA_MESSAGE_CONTROL_RELAY;
    lora_message.data = message;

    SerializedMessage serialized_message;
    serializeLoraMessage(serialized_message, lora_message);

    send_buffer->push_back(String(serialized_message.data, serialized_message.length));
    vTaskDelay(pdMS_TO_TICKS(1000));
  }
}

void readDataFromSerial(void *pvParameter)
{
  // Set up
  String buffer = "";
  char end_chr = '\n';
  bool read_done = false;

  while (true)
  {
    while (Serial.available()) {
      // Read data and save to buffer
      char character = (char)Serial.read();
      if (character == end_chr) {
        read_done = true;
        break;
      }
      buffer += character;
    }

    if (read_done) {
      read_done = false;
      // TODO: process data here
      if (buffer == "scan") {
        // Scan node
        scanNode(pvParameter);
      }
      
      // end TODO
      buffer = "";
    }
    vTaskDelay(pdMS_TO_TICKS(1000));
  }
}

/* Setup function */
void setup()
{
  // Initialize Pin and Serial
  Serial.begin(UART_DEFAUT_BAUDRATE, SERIAL_8N1, UART_RXD_DEBUG_PIN, UART_TXD_DEBUG_PIN);
  initDebugSerial(&Serial);
  pinMode(INBUILD_LED_PIN, OUTPUT);
  digitalWrite(INBUILD_LED_PIN, LOW);   // Turn off the build-in LED

  // Initialize watchdog
  initWatchdogTimer(RESET_WATCHDOG_TIME);

  // Initialize DHT20
  // initDHT20();

  // Initialize LoRa
  initLora();
  setConfiguration(NODE, 0x0005);   // Hard code with address node: 0x0002

  // Initialize Network layer and Device layer
  device_init();

  // Initialize queue for sending and receiving data
  BasicQueue<String> *send_queue = new BasicQueue<String>();
  BasicQueue<String> *recv_queue = new BasicQueue<String>();

  // Create task for RTOS
  xTaskCreate(handleProcessBuffer, "handle process buffer", 1024 * 8, recv_queue, 1, nullptr);
  xTaskCreate(LoRaRecvTask, "receiver", 1024*4, recv_queue, 0, nullptr);
  xTaskCreate(sendLoraMessageTask, "sender", 1024*4, send_queue, 1, nullptr);
  // xTaskCreate(produceMessage, "producer", 1024 * 2, send_queue, 2, nullptr);
  xTaskCreate(readDataFromSerial, "read data from keyboard", 1024 * 2, send_queue, 1, nullptr);

  // xTaskCreate(readDataDHT20, "DHT20 data reader", 1024 * 4, nullptr, 1, nullptr);

  digitalWrite(INBUILD_LED_PIN, HIGH);    // Turn on the LED when set up completely
}

/* Loop function */
void loop()
{
  // put your main code here, to run repeatedly:
}
