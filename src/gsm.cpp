#include "gsm.h"
#include "panel.h"

using namespace cek::ws_bus;

GsmCustomClient* gsmClient;

bool cek::unloadGSMModule()
{
    if(gsmClient)
        delete gsmClient;
    return true;
}

GsmCustomClient *cek::getModule()
{
    if (gsmClient == nullptr){
        gsmClient = GsmCustomClient::create(Serial2);
    }
    return gsmClient;
}

cek::ws_bus::EventCallback OnBalanceUpdate = []() {
    notify(eEventType::GsmUpdateStatus, cek::getModule()->getUSSD("*100#"));
};

cek::ws_bus::EventCallback OnStatusUpdate = []() {
    auto info = cek::getModule()->getModemInfo();
    notify(eEventType::GsmUpdateStatus, info);
};

cek::ws_bus::EventCallback OnBatteryUpdate = []() {    
    notify(eEventType::GsmUpdateBattPercent, cek::getModule()->getBattPercent());
};

cek::ws_bus::EventCallback OnSignalQualityUpdate = []() {
    notify(eEventType::GsmUpdateSignalQuality, cek::getModule()->getSignalQuality());
};

cek::ws_bus::EventCallback OnSendSMS = []() {
    bool result = cek::getModule()->sendSMS("+79081608348","testing esp32");
    notify(eEventType::GsmSendSMS, String(result));
};

bool cek::loadGSMModule()
{
    while (!getModule()->isDeviceConnected()){
        Serial.println ("GSM not yet connected");
        delay(2000);
    }
    //
    // .. нужно проверять сеть
    //

    getModule()->detectOperatorIMSI();
    Serial.println("Сеть: " + getModule()->isNetworkConnected());
    // регистрация обработчиков
    registerEventCallback(SubscibeId(eEventType::GsmUpdateStatus), OnStatusUpdate);
    registerEventCallback(SubscibeId(eEventType::GsmUpdateSignalQuality), OnSignalQualityUpdate);
    registerEventCallback(SubscibeId(eEventType::GsmUpdateBattPercent), OnBatteryUpdate);
    registerEventCallback(SubscibeId(eEventType::GsmUpdateBalance), OnBalanceUpdate);
    registerEventCallback(SubscibeId(eEventType::GsmSendSMS), OnSendSMS);

    return true;
}