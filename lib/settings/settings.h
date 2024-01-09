#ifndef _CEK_ESP_SETTINGS_H_
#define _CEK_ESP_SETTINGS_H_

#include "bufferSerializer.h"

namespace cek{

struct WifiSettings
{
    String ssid;
    String pwd;
 
    const BufferSerializer serialize();
    static WifiSettings deSerialize(BufferSerializer&& s);
};

struct MqttSettings
{
    String host;
    String port;
    String user;
    String pwd;
 
    const BufferSerializer serialize();
    static MqttSettings deSerialize(BufferSerializer& s);
};

class EepromSettings
{
public:
    // несколько wifi сетей
    WifiSettings wifis[2]; // last 'n=2' nets
    MqttSettings mqtt;
    // версия прошивки
    uint version;
    // Название прошивки
    String name;

    EepromSettings();
    //
    bool save();

    private:
    uint serialize(uint address, BufferSerializer& buf);

    
};

/**
 * Загружает настройки и EEPROM
*/
bool loadSettings();

/**
 * Возврвщает структуру с настройками. Предварительно нужно загрузить через loadSettings()
*/
EepromSettings& getEepromSettings();

}
#endif