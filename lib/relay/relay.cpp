#include "relay.h"

void cek::RelayOne::toogleRelay()
{
    auto val = digitalRead(gpio);
    turnRelay(val);
}

void cek::RelayOne::turnRelay(bool turnOn)
{
    if(turnOn){
        digitalWrite(gpio, LOW); // Записываем в PIN10 высокий уровень
    }else
        digitalWrite(gpio, HIGH);
}

const bool cek::RelayOne::isTurnOn()
{
    return digitalRead(gpio);
}

void cek::RelayOne::setPin(uint8_t gpio)
{
    this->gpio=gpio;
    pinMode(gpio, OUTPUT); // Настраиваем PIN10 на выход
    turnRelay(false);
}

const uint8_t cek::RelayOne::getPin()
{
    return gpio;
}
