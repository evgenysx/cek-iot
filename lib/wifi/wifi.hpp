#ifndef _CEK_ESP_WIFI_H_
#define _CEK_ESP_WIFI_H_

#include <Arduino.h>
#include "WiFi.h"


void initWifi(String ssid, String password){
  WiFi.begin();             // Connect to the network
  while (WiFi.status() != WL_CONNECTED) { // Wait for the Wi-Fi to connect
    delay(500);
    Serial.print('.');
  }
  Serial.println('\n');
  Serial.println("Connection established");  
  Serial.print("IP address:\t");
  Serial.println(WiFi.localIP()); 
}

/*
  Точка доступа для сервисных действий
*/
void initAPWifi(String uid){
  const char* ssidAP     = "cek_solar_";
  const char* passwordAP = "123456789";
  
   WiFi.softAP(ssidAP + uid, passwordAP);
 
  IPAddress IP = WiFi.softAPIP();
  Serial.print("AP IP address: ");
  Serial.println(IP);
}

#endif

