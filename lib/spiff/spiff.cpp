#include "spiff.h"
#include "SPIFFS.h"

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