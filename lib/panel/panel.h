
#ifndef _CEK_ESP_GUIPANEL_H_
#define _CEK_ESP_GUIPANEL_H_

#include <ESPAsyncWebServer.h>
#include <ArduinoJson.h>

#include "event_types.h"

namespace cek::ws_bus{
    
void onWsEvent(AsyncWebSocket * server, AsyncWebSocketClient * client, AwsEventType type, void * arg, uint8_t *data, size_t len);

void registerEventCallback(SubscibeId type, EventCallback callback);

void notify(const EventMsg& msg);

void notify(eEventType type, const String& msg);
void notify(eEventType type, const DynamicJsonDocument& msg);
/**
 * Уведомление об ошибке
*/
void notifyError(eEventType type, const String& msg);
/**
 * Уведомление об успешеной операции
*/
void notify(eEventType type, uint msg);

void startHttpServer();
/**
 * Создание нового json-ответа на сообщение
*/
DynamicJsonDocument wsResponse(eEventType type, eEventResult bSuccess);

void registerHandler(String url);

/**
 * отладочное сообщение в консоль сервера и в админку
*/
void debugInfo(const String& msg);

} // end namespace
#endif