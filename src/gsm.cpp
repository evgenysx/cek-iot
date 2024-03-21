#include "gsm.h"
#include "panel.h"

using namespace cek::ws_bus;

GsmCustomClient* gsmClient;

// требуется ли автообновление статуса GSM сети
bool enableGsmUpdateStatus = true;

bool restartModem();
bool startModem();

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
    cek::getModule()->updateBalance();
    //notify(eEventType::GsmUpdateBalance, );
};

cek::ws_bus::EventCallback OnStatusUpdate = [](JsonObject*) {
    auto info = cek::getModule()->isDeviceConnected();
    notify(eEventType::GsmUpdateStatus, info);
};

cek::ws_bus::EventCallback OnBatteryUpdate = [](JsonObject*) {    
    notify(eEventType::GsmUpdateBattPercent, cek::getModule()->getBattVoltage());
};

cek::ws_bus::EventCallback OnSignalQualityUpdate = [](JsonObject*) {
    cek::getModule()->updateSignalQuality();
};

cek::ws_bus::EventCallback OnSendSMS = [](JsonObject* data) {
     String to = (*data)["to"];
     String text = (*data)["text"];
    if(to.length() != 12){
        notifyError(eEventType::GsmSendSMS, "bad phone format");
        return;
    }
    if(text.length() > 140){
        notifyError(eEventType::GsmSendSMS, "too long sms");
        return;
    }
    cek::getModule()->registerSms(to, text);
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
    cek::getModule()->sendAT(cmd, 1000L);
    // String response;
    // auto res = cek::getModule()->getATResponse();
    // debugInfo("Response ATCmd: " + cmd + " / " + String(res.code) + " / " + res.value);
};

cek::ws_bus::EventCallback OnGsmEnableUpdateNetworkInfo = [](JsonObject* data) {
    bool value = (*data)["value"];
    enableGsmUpdateStatus = value;
};

OnUserRegStatus OnRegStatus = [](RegStatus status) {
    Serial.println("OnRegStatus got " + String(status));
};

OnUserStrCallback OnUserSignalQuality = [](String data) {
    notify(eEventType::GsmUpdateSignalQuality, data);
};

OnUserStrCallback OnUserBalanceUpdate = [](String data) {
    notify(eEventType::GsmUpdateBalance, data);
};

OnUserStr2Callback OnNetworkInfoEvent = [](String& key, String& value) {
    constexpr int bufSz = 128;
    DynamicJsonDocument doc(bufSz);
    doc["key"] = key;
    doc["value"] = value;
    char buf[bufSz];
    serializeJson(doc, buf);
    notify(eEventType::GsmNetworkInfo, buf);
};

OnUserDataCallback OnUserSmsDeliveryReportEvent = [](void* data) {
    constexpr int bufSz = 128;
    DynamicJsonDocument doc(bufSz);
    auto pData = (SmsReportDelivery*)data;
    doc["mr"] = pData->mr;
    doc["status"] = pData->status;
    doc["delivered"] = pData->deliveryDate;

    notify(eEventType::GsmSMSDeliveryReport, doc);
};

/**
 * mr - message reference
*/
OnUserStrCallback OnUserSmsSentEvent = [](String mr) {
    notify(eEventType::GsmSendSMS, mr);
};

cek::ws_bus::EventCallback OnNetworkInfo = [](JsonObject*) {
   String key = "operator";
   String data = cek::getModule()->getOperatorName();
   OnNetworkInfoEvent(key, data);
};

bool cek::loadGSMModule()
{
    registerEventCallback(SubscibeId(eEventType::GsmUpdateStatus), OnStatusUpdate);
    //
    //startModem();
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

    //
    cek::getModule()->setOnUserRegStatus(OnRegStatus);  
    //cek::getModule()->setOnUserSignalQuality(OnUserSignalQuality);
    cek::getModule()->setOnUserBalanceUpdate(OnUserBalanceUpdate);
    cek::getModule()->setOnUserNetworkUpdate(OnNetworkInfoEvent);
    cek::getModule()->setOnUserSmsDeliveryReport(OnUserSmsDeliveryReportEvent);
    cek::getModule()->setOnUserSmsSent(OnUserSmsSentEvent);
    //cek::getModule()->sendAT("+CREG=1");
    return true;
}

bool restartModem(){
    debugInfo("Restart gsm module");
    return cek::getModule()->restart();
}

bool startModem(){
    debugInfo("Init gsm module");
    return cek::getModule()->init();
}

unsigned long start = millis();
const auto MAX_GSM_NETWORK_IDLE_TIMEOUT = 4000;

/**
 * мониторинг состояния сети
 * */ 
void cek::GsmNetworkLoop(){
    // цикл задач
    cek::getModule()->taskLoop();
    
//    if (!enableGsmUpdateStatus)
//      return;
   if (millis() - start < MAX_GSM_NETWORK_IDLE_TIMEOUT)
    return;

  Serial.println("networkLoop " + String(cek::getModule()->getRegStatus()));

  start = millis();
}