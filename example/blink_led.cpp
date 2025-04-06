#include <globals.h>

void setup() 
{
    Serial.begin(UART_DEFAUT_BAUDRATE, SERIAL_8N1, UART_RXD_DEBUG_PIN, UART_TXD_DEBUG_PIN);
    initDebugSerial(&Serial);

    // Create task for RTOS
    xTaskCreate(taskLedBlink, "Task Blinky Led", 1024, nullptr, 2, nullptr);
}

void loop()
{
    // Do nothing
}