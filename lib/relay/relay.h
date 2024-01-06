#ifndef _CEK_ESP_RELAY_H_
#define _CEK_ESP_RELAY_H_

#include <Arduino.h>

namespace cek{

class RelayOne {
public:

void toogleRelay();

void turnRelay(bool turnOn);
const bool isTurnOn();

void setPin(uint8_t gpio);
const uint8_t getPin();

private:
    uint8_t gpio;
};

} // end nanespace
#endif