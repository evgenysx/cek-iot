#include "event_types.h"
#include <map>


cek::ws_bus::EventMsg::EventMsg(eEventType type)
{
    this->type = type;
    this->id = noSelectedId;
}

cek::ws_bus::EventMsg::EventMsg()
{
  id = noSelectedId;
}

String strEventType[] PROGMEM = {
  "Unknown",
  "GsmUpdateStatus",
  "GsmUpdateBalance",
  "GsmUpdateSignalQuality",  
  "GsmUpdateBattPercent",
  "GsmSendSMS",
  "GsmCallNumber",
  "GsmNetworkInfo",
  "GsmGetLocation",
  "GsmRestartModem",
  "GsmATCmd",
  "GsmEnableUpdateNetworkInfo",
  "RelayToggle",
  "EnableLog",
  "DisableLog",
  "PrintLog",
  "RestartDevice"
};

std::map<String, cek::ws_bus::eEventType> reverseEventTypesStore;

String cek::ws_bus::getStrEventType(eEventType type)
{
  return strEventType[(int)type];
}

cek::ws_bus::eEventType cek::ws_bus::getEventTypeByStr(String strEvent){
  auto iter = reverseEventTypesStore.find(strEvent);
    if (iter != reverseEventTypesStore.end()) {  
       return iter->second;
    }else
      return cek::ws_bus::eEventType::Unknown;
}

cek::ws_bus::SubscibeId::SubscibeId(eEventType type, int id){
  this->type = type;
  this->id = id;
}

cek::ws_bus::SubscibeId::SubscibeId(eEventType type){
  this->type = type;
  this->id = noSelectedId;
}

void cek::ws_bus::initEventMap(){
  using namespace cek::ws_bus;
  for (int i = 0;i < (int)eEventType::NUM_EVENTS; i++){
    auto event = (eEventType)i;
    reverseEventTypesStore.insert(std::pair<String, eEventType>{getStrEventType(event), event});
  }
}
