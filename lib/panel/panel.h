
#ifndef _CEK_ESP_GUIPANEL_H_
#define _CEK_ESP_GUIPANEL_H_

#include <ESPAsyncWebServer.h>
#include <ArduinoJson.h>

#include "event_types.h"

namespace cek::ws_bus{
    
// обратные вызовы
typedef std::function<void()> EventCallback;

void onWsEvent(AsyncWebSocket * server, AsyncWebSocketClient * client, AwsEventType type, void * arg, uint8_t *data, size_t len);

void registerEventCallback(const char* id, EventCallback* callback);

void notify(const EventMsg& msg);
void notify(eEventType type, uint msg);

void startHttpServer();


void registerHandler(String url);


} // end namespace
#endif