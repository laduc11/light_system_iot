#include "serial_print.h"

void printData(String FeedName, String message)
{
    Serial.print(message);
#ifdef _DEBUG_MODE_
    Serial.print(message);
#endif
}

void printlnData(String FeedName, String message)
{
    Serial.println(message);
#ifdef _DEBUG_MODE_
    Serial.println(message);
#endif
}