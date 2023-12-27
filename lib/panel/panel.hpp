
#ifndef _CEK_ESP_GUIPANEL_H_
#define _CEK_ESP_GUIPANEL_H_

#include "wifi.hpp"
#include <ESPAsyncWebServer.h>

AsyncWebServer server(80);

const char* ssidAP     = "cek_solar_";
const char* passwordAP = "123456789";

#include "SPIFFS.h"


void startHttpServer(){
 // Define a route to serve the HTML page
  server.on("/", HTTP_GET, [](AsyncWebServerRequest* request) {
    Serial.println("ESP32 Web Server: New request received:");  // for debugging
    request->send(SPIFFS, "/index.html");
    //request->send(200, "text/html", "<html><body><h1>Hello, ESP32!</h1></body></html>");
  });

  server.on("/style.css", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send(SPIFFS, "/style.css", "text/css");
  });

  SPIFFS.begin();
  // Start the server
  server.begin();
}


void registerHandler(String url){
    server.on("/api/sendSMS", HTTP_POST, [](AsyncWebServerRequest *request){
        Serial.println("/api/sendSMS");
        request->send(200, "text/html", "test");
    });
}

void testSPIFF(){
    if (!SPIFFS.begin())
    {
        Serial.println("An Error has occurred while mounting SPIFFS");
        return;
    }
    File root = SPIFFS.open("/");
    	

//     File file = SPIFFS.open("/test.txt", FILE_WRITE);

//     if (!file) {
//         Serial.println("There was an error opening the file for writing");
//         return;
//     }
//     if (file.print("TEST")) {
//         Serial.println("File was written");
//     } else {
//         Serial.println("File write failed");
//     }

//   file.close();

    //auto res = SPIFFS.mkdir("gui");
    
    //Serial.println("Creating folder " + String(root.available()));

    File fileNext = root.openNextFile();
    while(fileNext){
        Serial.print("FILE: ");
        Serial.println(fileNext.name());
        fileNext = root.openNextFile();
    }

    SPIFFS.end();
}

#endif