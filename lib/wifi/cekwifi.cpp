#include "cekwifi.h"

/**
 * Подключение к существующей точке WiFi
*/
bool initWifi(String ssid, String password){
  uint trys = 5;
  WiFi.begin(ssid, password);             // Connect to the network
  while (WiFi.status() != WL_CONNECTED) { // Wait for the Wi-Fi to connect
    delay(1000);
    Serial.print('.');
    if(!trys--){
      Serial.println("Connection failed");
      return false;
    }
  }
  Serial.println('\n');
  Serial.println("Connection established");  
  Serial.print("IP address:\t");
  Serial.println(WiFi.localIP()); 
  return true;
}

/**
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

