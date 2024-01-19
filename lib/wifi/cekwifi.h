#ifndef _CEK_ESP_WIFI_H_
#define _CEK_ESP_WIFI_H_

#include <Arduino.h>
#include <WiFi.h>

/**
 * Подключение к существующей точке WiFi
*/
bool initWifi(String ssid, String password);

/**
  Точка доступа для сервисных действий
*/
bool initAPWifi(String uid);

#endif

