
#ifndef _CEK_ESP_GUIPANEL_H_
#define _CEK_ESP_GUIPANEL_H_

#include "wifi.h"
#include <ESPAsyncWebServer.h>
#include <ArduinoJson.h>


namespace cek{
// обратные вызовы
typedef std::function<void()> EventCallback;

void onWsEvent(AsyncWebSocket * server, AsyncWebSocketClient * client, AwsEventType type, void * arg, uint8_t *data, size_t len);

void registerEventCallback(const char* id, EventCallback* callback);

void notify();

void startHttpServer();


void registerHandler(String url);

void testSPIFF();

} // end namespace
#endif