#ifndef _CEK_ESP_GSM_MODULE_H_
#define _CEK_ESP_GSM_MODULE_H_

#include "relay.h"
#include "gprs.h"

namespace cek {

bool loadGSMModule();
bool unloadGSMModule();

GsmCustomClient* getModule();
} // end namespace 
#endif