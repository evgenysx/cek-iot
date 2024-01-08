
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
};

String getStrEventType(eEventType type);

struct EventMsg {
  eEventType type;
  String data;

  EventMsg(eEventType type, String data);
};

typedef std::function<void()> EventCallback;

};
#endif