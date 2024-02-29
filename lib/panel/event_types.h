
#ifndef _CEK_ESP_WS_TYPES_H_
#define _CEK_ESP_WS_TYPES_H_

#include <Arduino.h>
#include <ArduinoJson.h>

namespace cek::ws_bus {

enum class eEventType {
  Unknown = 0,
  GsmUpdateStatus,
  GsmUpdateBalance,
  GsmUpdateSignalQuality,  
  GsmUpdateBattPercent,
  GsmSendSMS,
  GsmSMSDeliveryReport,
  GsmCallNumber,
  GsmNetworkInfo,
  GsmGetLocation,
  GsmRestartModem,
  GsmATCmd,
  GsmEnableUpdateNetworkInfo,
  // реле
  RelayToggle,
  // вывод отладочной информации
  EnableLog,
  DisableLog,
  PrintLog,
  // перезагрузка устройства
  RestartDevice,
  NUM_EVENTS
};

enum class eEventResult {
  Success = 0,
  Error
};

String getStrEventType(eEventType type);
eEventType getEventTypeByStr(String);
void initEventMap();

static const int noSelectedId = -1;

struct SubscibeId {
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
  JsonObject data;

  EventMsg(eEventType type);
  EventMsg();
};

typedef void (*EventCallback) (JsonObject* data);

};
#endif