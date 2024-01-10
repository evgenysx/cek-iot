#include "sensors.h"
#include "panel.h"



cek::RelayOne relay23;

using namespace cek::ws_bus;
EventCallback RelayToggle = []() {
    Serial.println("RelayToggle");
    relay23.toogleRelay();
};


void cek::sensors::init(){
    relay23.setPin(23);
    registerEventCallback(SubscibeId(eEventType::RelayToggle, relay23.getPin()), RelayToggle);
}