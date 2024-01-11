

#include "panel.h"

#include <ESPAsyncWebServer.h>
#include <ArduinoJson.h>


#include <map>

std::map<cek::ws_bus::SubscibeId, cek::ws_bus::EventCallback> storeEvents;

AsyncWebServer server(80);
AsyncWebSocket ws("/ws");



#include "SPIFFS.h"

DynamicJsonDocument doc(128);

cek::ws_bus::SubscibeId parseMsg () {
    const char* strType = doc["type"];
    int id = cek::ws_bus::SubscibeId::noSelectedId; 
    if(doc.containsKey("id"))
      id = doc["id"];
    return cek::ws_bus::SubscibeId(cek::ws_bus::getEventTypeByStr(strType), id);
}

void cek::ws_bus::onWsEvent(AsyncWebSocket * server, AsyncWebSocketClient * client, AwsEventType type, void * arg, uint8_t *data, size_t len){
 
  if(type == WS_EVT_CONNECT){ 
    Serial.println("New ws client");
  } else if(type == WS_EVT_DATA){
    doc.clear();
    DeserializationError error = deserializeJson(doc, data);
    if (error)
        Serial.println(F("Failed to read file, using default configuration"));

    auto subInfo = parseMsg();
    auto iter = storeEvents.find(subInfo);
    if (iter != storeEvents.end()) {
        Serial.println("Handle :" + getStrEventType(subInfo.type) + " / " + String(subInfo.id));
       (*iter->second)();
    }else{
      Serial.println("No any handlers for event '" + getStrEventType(subInfo.type) + "'");
    }
     

  } else if(type == WS_EVT_DISCONNECT){
    Serial.println("Client ws disconnected");
  }
}

void cek::ws_bus::notify(const EventMsg& msg){
  DynamicJsonDocument doc(128);
  doc["type"] = getStrEventType(msg.type);
  doc["data"] = msg.data;

  //const size_t len = measureJson(doc);
  char buf[128];
  serializeJson(doc, buf);
  Serial.println(buf);
  ws.textAll(buf);
}

void cek::ws_bus::notify(eEventType type, uint msg)
{
  DynamicJsonDocument doc(128);
  doc["type"] = getStrEventType(type);
  doc["data"] = msg;

  //const size_t len = measureJson(doc);
  char buf[128];
  serializeJson(doc, buf);
  Serial.println(buf);
  ws.textAll(buf);
}

void cek::ws_bus::notify(eEventType type, String msg)
{
  DynamicJsonDocument doc(128);
  doc["type"] = getStrEventType(type);
  doc["data"] = msg;

  //const size_t len = measureJson(doc);
  char buf[128];
  serializeJson(doc, buf);
  Serial.println(buf);
  ws.textAll(buf);
}

void cek::ws_bus::startHttpServer(){
 // Define a route to serve the HTML page
  server.on("/", HTTP_GET, [](AsyncWebServerRequest* request) {
    Serial.println("ESP32 Web Server: New request received:");  // for debugging
    request->send(SPIFFS, "/index.html");
  });

  server.on("/style.css", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send(SPIFFS, "/style.css", "text/css");
  });

  server.on("/css/switch.css", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send(SPIFFS, "/css/switch.css", "text/css");
  });

  server.on("/dist/bundle.js", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send(SPIFFS, "/dist/bundle.js", "application/javascript");
  });

  // инициализация списка событий
  initEventMap();

  ws.onEvent(onWsEvent);
  server.addHandler(&ws);

  SPIFFS.begin();
  // Start the server
  server.begin();
}


void cek::ws_bus::registerHandler(String url){
    server.on("/api/sendSMS", HTTP_POST, [](AsyncWebServerRequest *request){
        Serial.println("/api/sendSMS");
        request->send(200, "text/html", "test");
    });
}



void cek::ws_bus::registerEventCallback(SubscibeId id, EventCallback callback)
{
  storeEvents.insert(std::pair<SubscibeId, EventCallback>{id, callback});
}
