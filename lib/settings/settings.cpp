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

WifiSettings WifiSettings::deSerialize(BufferSerializer&& s){
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
  BufferSerializer wifiBufs(sz);
  EEPROM.readBytes(address, (byte*)wifiBufs.getBytes(), sz);
  address += sz;
  //
  auto lenWifi = wifiBufs.readUint();
  for(auto i=0;i<lenWifi;i++){
    info.wifis[i] = WifiSettings::deSerialize(wifiBufs.readBuf());
  }
  
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
  // несколько wifi сетей
  BufferSerializer wifiBufs;
  // total nets
  wifiBufs.put(sizeof(wifis)/sizeof(wifis[0]));
  for (auto iWifi : wifis){
    auto wifiBuf = iWifi.serialize();
    wifiBufs.put(wifiBuf);
  }
  
  auto mqttBuf = mqtt.serialize();

  const auto totalSz = sizeof(MAGIC_SETTINGS) + (name.length() +1)
      + sizeof(version)
      + wifiBufs.getSize() + sizeof(size_t)
      + mqttBuf.getSize() + sizeof(size_t);

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
  address = serialize(address, wifiBufs);
  // данные mqtt
  address = serialize(address, mqttBuf);
  // контрольная проверка
  if (totalSz != address){
    return false;
  }

  EEPROM.end();
  return true;
}

uint EepromSettings::serialize(uint address,BufferSerializer& buf){
  auto sz = buf.getSize();
  EEPROM.writeUInt(address, sz);
  address += sizeof(sz);
  EEPROM.writeBytes(address, buf.getBytes(), sz);
  address += sz;
  return address;
}

const BufferSerializer cek::MqttSettings::serialize()
{
    BufferSerializer buf;
    buf.put(host);
    buf.put(port);
    buf.put(user);
    buf.put(pwd);
    return buf;
}

MqttSettings MqttSettings::deSerialize(BufferSerializer& s){
  MqttSettings mqttSet;
  mqttSet.host = s.readString();
  mqttSet.port = s.readString();
  mqttSet.user = s.readString();
  mqttSet.pwd = s.readString();
  return mqttSet;
}