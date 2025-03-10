#include "print_debug.h"

HardwareSerial *serial_ = nullptr;

void initDebugSerial(HardwareSerial *serial)
{
#ifdef _DEBUG_MODE_
if (nullptr != serial) {
    serial_ = serial;
}
#endif  // _DEBUG_MODE_
}

void printData(String message)
{
#ifdef _DEBUG_MODE_
    if (nullptr == serial_) {
        return;
    }
    serial_->print(message);
#endif  // _DEBUG_MODE_
}

void printlnData(String message)
{
#ifdef _DEBUG_MODE_
    if (nullptr == serial_) {
        return;
    }
    serial_->println(message);
#endif  // _DEBUG_MODE_
}

void printfData(String message)
{
#ifdef _DEBUG_MODE_
    if (nullptr == serial_) {
        return;
    }
    serial_->println(message);
#endif  // _DEBUG_MODE_
}

HardwareSerial * getDebugSerial()
{
#ifdef _DEBUG_MODE_
    return serial_;
#else
    return nullptr;
#endif  // _DEBUG_MODE_
}