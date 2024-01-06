

#include "panel.h"

#include <ESPAsyncWebServer.h>
#include <ArduinoJson.h>


#include <map>
typedef std::function<void()> EventCallback;
std::map<String, EventCallback*> storeEvents;

AsyncWebServer server(80);
AsyncWebSocket ws("/ws");

#include "SPIFFS.h"

DynamicJsonDocument doc(128);

void cek::onWsEvent(AsyncWebSocket * server, AsyncWebSocketClient * client, AwsEventType type, void * arg, uint8_t *data, size_t len){
 
  if(type == WS_EVT_CONNECT){ 
    Serial.println("New ws client");
 
  } else if(type == WS_EVT_DATA){
    doc.clear();
    //StaticJsonDocument<len> doc;
    DeserializationError error = deserializeJson(doc, data);
    if (error)
        Serial.println(F("Failed to read file, using default configuration"));

    const char* id = doc["id"];
    auto iter = storeEvents.find(id);
    if (iter != storeEvents.end()) {
       (*iter->second)();
    }
     

  } else if(type == WS_EVT_DISCONNECT){
    Serial.println("Client ws disconnected");
  }
}

void cek::notify(){

}

void cek::startHttpServer(){
 // Define a route to serve the HTML page
  server.on("/", HTTP_GET, [](AsyncWebServerRequest* request) {
    Serial.println("ESP32 Web Server: New request received:");  // for debugging
    request->send(SPIFFS, "/index.html");
    //request->send(200, "text/html", "<html><body><h1>Hello, ESP32!</h1></body></html>");
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

  ws.onEvent(onWsEvent);
  server.addHandler(&ws);

  SPIFFS.begin();
  // Start the server
  server.begin();
}


void cek::registerHandler(String url){
    server.on("/api/sendSMS", HTTP_POST, [](AsyncWebServerRequest *request){
        Serial.println("/api/sendSMS");
        request->send(200, "text/html", "test");
    });
}

void cek::testSPIFF(){
    if (!SPIFFS.begin())
    {
        Serial.println("An Error has occurred while mounting SPIFFS");
        return;
    }
    File root = SPIFFS.open("/");

    File fileNext = root.openNextFile();
    while(fileNext){
        Serial.print("FILE: ");
        Serial.println(fileNext.name());
        fileNext = root.openNextFile();
    }

    SPIFFS.end();
}

void cek::registerEventCallback(const char* id, EventCallback *callback)
{
  storeEvents.insert(std::pair<String, EventCallback*>{id, callback});
}
