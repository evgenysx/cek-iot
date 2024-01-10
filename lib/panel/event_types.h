
#ifndef _CEK_ESP_WS_TYPES_H_
#define _CEK_ESP_WS_TYPES_H_

#include <Arduino.h>

namespace cek::ws_bus {

enum class eEventType {
  Unknown = 0,
  GsmUpdateStatus,
  GsmUpdateBalance,
  GsmUpdateSignalQuality,  
  GsmUpdateBattPercent,
  GsmSendSMS,
  GsmCallNumber,
  RelayToggle,

  NUM_EVENTS
};

String getStrEventType(eEventType type);
eEventType getEventTypeByStr(String);
void initEventMap();


struct SubscibeId {
  static const int noSelectedId = -1;
  
  eEventType type;
  int id;

  bool const operator==(const SubscibeId &o) {
        return type == o.type && id == o.id;
    }

  bool const operator<(const SubscibeId &o) const {
      return type < o.type || (type == o.type && id < o.id);
  }

  SubscibeId(eEventType type, int id);
  SubscibeId(eEventType type);
};

struct EventMsg {
  eEventType type;
  int id;
  String data;

  EventMsg(eEventType type, String data);
};

typedef void (*EventCallback) ();

};
#endif