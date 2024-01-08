#include "event_types.h"

cek::ws_bus::EventMsg::EventMsg(eEventType type, String data)
{
    this->type = type;
    this->data = data;
}

String strEventType[] PROGMEM = {
  "Unknown",
  "GsmUpdateStatus",
  "GsmUpdateBalance",
  "GsmUpdateSignalQuality",  
  "GsmUpdateBattPercent",
};

String cek::ws_bus::getStrEventType(eEventType type)
{
  return strEventType[(int)type];
}
