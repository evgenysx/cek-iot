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

void cek::getPartions(){
    auto part_iter = esp_partition_find(ESP_PARTITION_TYPE_ANY, ESP_PARTITION_SUBTYPE_ANY, NULL);
    while(part_iter != NULL){
      auto part_info = esp_partition_get(part_iter);
      Serial.println(part_info->label);
      Serial.println(part_info->type);
      Serial.println(part_info->address, HEX);
      Serial.println(part_info->size, HEX);
      Serial.println(part_info->subtype);
      Serial.println("============================");
      part_iter = esp_partition_next(part_iter);
    }
}