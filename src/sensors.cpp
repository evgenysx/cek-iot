#include "sensors.h"
#include "panel.h"

cek::RelayOne relay23;

std::function<void()> f = []() {
    relay23.toogleRelay();
};

void cek::sensors::init(){
    relay23.setPin(23);
    cek::ws_bus::registerEventCallback("relay_enabled", &f);
}