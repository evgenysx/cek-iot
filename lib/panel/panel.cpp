

#include "panel.h"

#include <ESPAsyncWebServer.h>
#include <ArduinoJson.h>


#include <map>

std::map<cek::ws_bus::SubscibeId, cek::ws_bus::EventCallback> storeEvents;

AsyncWebServer server(80);
AsyncWebSocket ws("/ws");

// нужно поместить в настройки прошивки
bool enabledLog = true;


#include "SPIFFS.h"


cek::ws_bus::EventMsg parseMsg (DynamicJsonDocument& doc) {
    const char* strType = doc["type"];
    cek::ws_bus::EventMsg msg;   
    if(doc.containsKey("id"))
      msg.id = doc["id"];
    msg.type = cek::ws_bus::getEventTypeByStr(strType);
    if(doc.containsKey("data")){
      msg.data = doc["data"].as<JsonObject>();
    }
    return msg;
}

void cek::ws_bus::onWsEvent(AsyncWebSocket * server, AsyncWebSocketClient * client, AwsEventType type, void * arg, uint8_t *data, size_t len){
 
  if(type == WS_EVT_CONNECT){ 
    Serial.println("New ws client");
  } else if(type == WS_EVT_DATA){
    DynamicJsonDocument doc(128);
    DeserializationError error = deserializeJson(doc, data);
    if (error){
        debugInfo(F("Failed to read file, using default configuration"));
        return;
    }
    auto msgInfo = parseMsg(doc);
    auto iter = storeEvents.find(cek::ws_bus::SubscibeId(msgInfo.type, msgInfo.id));
    if (iter != storeEvents.end()) {
        debugInfo("New event:" + getStrEventType(msgInfo.type) + " / "/* + msgInfo.data*/);
       (*iter->second)(&msgInfo.data);
    }else{
      debugInfo("No any handlers for event '" + getStrEventType(msgInfo.type) + "'");
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

void cek::ws_bus::notify(eEventType type, const String& msg)
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


void cek::ws_bus::debugInfo(const String& msg)
{
  Serial.println(msg);
  if(enabledLog)
    notify(eEventType::PrintLog, msg);
}

void cek::ws_bus::registerEventCallback(SubscibeId id, EventCallback callback)
{
  storeEvents.insert(std::pair<SubscibeId, EventCallback>{id, callback});
}
