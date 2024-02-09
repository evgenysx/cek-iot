#include "gsm.h"
#include "panel.h"

using namespace cek::ws_bus;

GsmCustomClient* gsmClient;

// требуется ли автообновление статуса GSM сети
bool enableGsmUpdateStatus = true;

bool restartModem();

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
    auto info = cek::getModule()->isDeviceConnected();
    notify(eEventType::GsmUpdateStatus, info);
};

cek::ws_bus::EventCallback OnBatteryUpdate = [](JsonObject*) {    
    notify(eEventType::GsmUpdateBattPercent, cek::getModule()->getBattVoltage());
};

cek::ws_bus::EventCallback OnSignalQualityUpdate = [](JsonObject*) {
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
    auto result = cek::getModule()->sendSMSinPDU(to, text);
    notify(eEventType::GsmSendSMS, String(result));
};

cek::ws_bus::EventCallback OnGetLocation = [](JsonObject*) {
    notify(eEventType::GsmGetLocation, cek::getModule()->getGsmLocation());
};

cek::ws_bus::EventCallback OnRestartModem = [](JsonObject*) {
    notify(eEventType::GsmRestartModem, restartModem());
};

cek::ws_bus::EventCallback OnGsmATCmd = [](JsonObject* data) {
    String cmd = (*data)["cmd"];
    Serial.println("cmd: " + cmd);
    cek::getModule()->sendAT(cmd, 10000L);
    // String response;
    // auto res = cek::getModule()->getATResponse();
    // debugInfo("Response ATCmd: " + cmd + " / " + String(res.code) + " / " + res.value);
};

cek::ws_bus::EventCallback OnGsmEnableUpdateNetworkInfo = [](JsonObject* data) {
    bool value = (*data)["value"];
    enableGsmUpdateStatus = value;
};


cek::ws_bus::EventCallback OnNetworkInfo = [](JsonObject*) {
    constexpr int bufSz = 256;
    DynamicJsonDocument doc(bufSz);
    //doc["network"] = cek::getModule()->isNetworkConnected();
    doc["reg"] = cek::getModule()->getRegStatus();
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
    registerEventCallback(SubscibeId(eEventType::GsmUpdateStatus), OnStatusUpdate);
    //
    restartModem();
    // регистрация обработчиков
    //
    registerEventCallback(SubscibeId(eEventType::GsmNetworkInfo), OnNetworkInfo);
    
    registerEventCallback(SubscibeId(eEventType::GsmUpdateSignalQuality), OnSignalQualityUpdate);
    registerEventCallback(SubscibeId(eEventType::GsmUpdateBattPercent), OnBatteryUpdate);
    registerEventCallback(SubscibeId(eEventType::GsmUpdateBalance), OnBalanceUpdate);
    registerEventCallback(SubscibeId(eEventType::GsmSendSMS), OnSendSMS);
    
    registerEventCallback(SubscibeId(eEventType::GsmGetLocation), OnGetLocation);
    registerEventCallback(SubscibeId(eEventType::GsmRestartModem), OnRestartModem);
    registerEventCallback(SubscibeId(eEventType::GsmATCmd), OnGsmATCmd);
    registerEventCallback(SubscibeId(eEventType::GsmEnableUpdateNetworkInfo), OnGsmEnableUpdateNetworkInfo);

    return true;
}

bool restartModem(){
    debugInfo("Restart gsm module");
    bool bRestart = cek::getModule()->restart();
    return bRestart;
}

unsigned long start = millis();
const auto MAX_GSM_NETWORK_IDLE_TIMEOUT = 2000;

/**
 * мониторинг состояния сети
 * */ 
void cek::GsmNetworkLoop(){
  if (!enableGsmUpdateStatus)
    return;
  if (millis() - start < MAX_GSM_NETWORK_IDLE_TIMEOUT)
    return;

  auto regStatus = cek::getModule()->getRegistrationStatus();

  Serial.println("networkLoop " + String(regStatus) + " / " + String(cek::getModule()->getRegStatus()));
  // check new status
  if (regStatus != cek::getModule()->getRegStatus()) {
    cek::getModule()->setRegStatus(regStatus);

    if(regStatus == RegStatus::REG_SEARCHING || regStatus == RegStatus::REG_OK_HOME ){
        cek::getModule()->detectOperatorIMSI();
    }else{
        cek::getModule()->setOperator(eGsmOperator::NotSelected);
    }
    
    OnNetworkInfo(nullptr);
  }else if (regStatus == RegStatus::REG_SEARCHING || regStatus == RegStatus::REG_OK_HOME){
    // статус сети не менялся - просто обновляем уровень сигнала
    OnSignalQualityUpdate(nullptr);
  }

    // String response;
    // auto res = cek::getModule()->waitResponse(1000L, response, GF("CDS"));
    // debugInfo("Unsolicited ATCmd: " + String(res) + " / " + response);
    // res = cek::getModule()->waitResponse(1000L, response, GF("CMTI"));
    // debugInfo("Unsolicited ATCmd: " + String(res) + " / " + response);

    // if (Serial2.available()) {                     // Если есть, что считывать...
    //     Serial.println(Serial2.readString());
    // }

  
    start = millis();
}