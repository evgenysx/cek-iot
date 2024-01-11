#include "esp32.h"
#include "panel.h"

#include <Arduino.h>

using namespace cek::ws_bus;


void cek::restartDevice(int delaySec){
    ESP.restart();
}

cek::ws_bus::EventCallback OnRestartDevice = []() {
    cek::restartDevice(0);
};

void cek::initDevice(){
    registerEventCallback(SubscibeId(eEventType::RestartDevice), OnRestartDevice);
}

