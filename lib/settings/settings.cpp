#include "settings.h"
#include <EEPROM.h>

using namespace cek;

EepromSettings info;


constexpr char MAGIC_SETTINGS[] = "CEK!";

const BufferSerializer WifiSettings::serialize(){
    BufferSerializer buf;
    buf.put(ssid);
    buf.put(pwd);
    return buf;
}

WifiSettings WifiSettings::deSerialize(BufferSerializer& s){
  WifiSettings wifiSet;
  wifiSet.ssid = s.readString();
  wifiSet.pwd = s.readString();
  return wifiSet;
}

bool cek::loadSettings()
{
  if(!EEPROM.begin(512))
    return false;

  int address = 0;
  if(!EEPROM.readString(address).equals(MAGIC_SETTINGS)){
    return false;
  }
  address += sizeof(MAGIC_SETTINGS);
  // название прошивки
  info.name = EEPROM.readString(address);
  address += info.name.length() + 1;
  // версия прошивки
  info.version = EEPROM.readUInt(address);
  address += sizeof(info.version);
  if (info.version > 9999)
    return false;
  // данные WiFi
  auto sz = EEPROM.readUInt(address);
  address += sizeof(sz);
  BufferSerializer wifiBuf(sz);
  EEPROM.readBytes(address, wifiBuf.getBytes(), sz);
  address += sz;
  //
  info.wifi = WifiSettings::deSerialize(wifiBuf);
  EEPROM.end();
  return true;
}

EepromSettings& cek::getEepromSettings()
{
  return info;
}

EepromSettings::EepromSettings() {
  //version = 1;
}

bool EepromSettings::save(){
  auto wifiBuf = wifi.serialize();

  const auto totalSz = sizeof(MAGIC_SETTINGS) + (name.length() +1)
      + sizeof(version)
      + wifiBuf.getSize() + sizeof(size_t);

  if(!EEPROM.begin(totalSz))
    return false;

  int address = 0;
  EEPROM.writeString(address, MAGIC_SETTINGS);
  address += sizeof(MAGIC_SETTINGS);
  // название прошивки
  EEPROM.writeString(address, name);
  address += name.length() + 1;
  // версия прошивки
  EEPROM.writeUInt(address, version);
  address += sizeof(version);
  // данные wifi
  auto sz = wifiBuf.getSize();
  EEPROM.writeUInt(address, sz);
  address += sizeof(sz);
  EEPROM.writeBytes(address, wifiBuf.getBytes(), sz);
  address += sz;
  
  // контрольная проверка
  if (totalSz != address){
    return false;
  }

  EEPROM.end();
  return true;
}