#include "gsm.h"
#include "panel.h"

using namespace cek::ws_bus;

GsmCustomClient* gsmClient;

bool restartModem(){
    bool bRestart = cek::getModule()->restart();
    cek::getModule()->detectOperatorIMSI();
    return bRestart;
}

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

cek::ws_bus::EventCallback OnBalanceUpdate = [](JsonObject*) {
    notify(eEventType::GsmUpdateBalance, cek::getModule()->getUSSD("*100#"));
};

cek::ws_bus::EventCallback OnStatusUpdate = [](JsonObject*) {
    auto info = cek::getModule()->getModemInfo();
    notify(eEventType::GsmUpdateStatus, info);
};

cek::ws_bus::EventCallback OnBatteryUpdate = [](JsonObject*) {    
    notify(eEventType::GsmUpdateBattPercent, cek::getModule()->getBattPercent());
};

cek::ws_bus::EventCallback OnSignalQualityUpdate = [](JsonObject*) {
    Serial.println(cek::getModule()->getSignalQuality());
    notify(eEventType::GsmUpdateSignalQuality, cek::getModule()->getSignalQuality());
};

cek::ws_bus::EventCallback OnSendSMS = [](JsonObject* data) {
     String to = (*data)["to"];
     String text = (*data)["text"];
    if(to.length() != 12){
        notify(eEventType::GsmSendSMS, "bad phone format");
        return;
    }
    if(text.length() > 70){
        notify(eEventType::GsmSendSMS, "too long sms");
        return;
    }
    bool result = cek::getModule()->sendSMS(to, text);
    notify(eEventType::GsmSendSMS, String(result));
};

cek::ws_bus::EventCallback OnGetLocation = [](JsonObject*) {
    notify(eEventType::GsmGetLocation, cek::getModule()->getGsmLocation());
};

cek::ws_bus::EventCallback OnRestartModem = [](JsonObject*) {
    notify(eEventType::GsmRestartModem, restartModem());
};

cek::ws_bus::EventCallback OnGsmATCmd = [](JsonObject* data) {
    //Serial.println("cmd: " + cmd);
    // cek::getModule()->sendAT(cmd);
    // auto res = cek::getModule()->waitResponse(10000L, response);
    // Serial.println("Response: " + String(res) + " / " + response);
};

cek::ws_bus::EventCallback OnNetworkInfo = [](JsonObject*) {
    constexpr int bufSz = 256;
    DynamicJsonDocument doc(bufSz);
    doc["network"] = cek::getModule()->isNetworkConnected();
    doc["reg"] = cek::getModule()->getRegistrationStatus();
    doc["operator"] = cek::getModule()->getOperatorName();
    doc["signal"] = cek::getModule()->getSignalQuality();

    
    //const size_t len = measureJson(doc);
    char buf[bufSz];
    serializeJson(doc, buf);
    Serial.println(buf);
    notify(eEventType::GsmNetworkInfo, buf);
};

bool cek::loadGSMModule()
{
    restartModem();

    while (!getModule()->isDeviceConnected()){
        Serial.println ("GSM not yet connected");
        notify(eEventType::GsmNetworkInfo, "{\"reg\":-1}");
        delay(2000);
    }

    // регистрация обработчиков
        //
    registerEventCallback(SubscibeId(eEventType::GsmNetworkInfo), OnNetworkInfo);
    registerEventCallback(SubscibeId(eEventType::GsmUpdateStatus), OnStatusUpdate);
    registerEventCallback(SubscibeId(eEventType::GsmUpdateSignalQuality), OnSignalQualityUpdate);
    registerEventCallback(SubscibeId(eEventType::GsmUpdateBattPercent), OnBatteryUpdate);
    registerEventCallback(SubscibeId(eEventType::GsmUpdateBalance), OnBalanceUpdate);
    registerEventCallback(SubscibeId(eEventType::GsmSendSMS), OnSendSMS);
    
    registerEventCallback(SubscibeId(eEventType::GsmGetLocation), OnGetLocation);
    registerEventCallback(SubscibeId(eEventType::GsmRestartModem), OnRestartModem);
    registerEventCallback(SubscibeId(eEventType::GsmATCmd), OnGsmATCmd);
    
    while (!getModule()->isNetworkConnected()){
         
          Serial.print ("." + String(getModule()->getRegistrationStatus()) + String(getModule()->getSignalQuality()) );
          OnNetworkInfo(nullptr);
          delay(2000);       
    }
    OnNetworkInfo(nullptr);
    Serial.println("READY = " + String(getModule()->getRegistrationStatus()));
    return true;
}