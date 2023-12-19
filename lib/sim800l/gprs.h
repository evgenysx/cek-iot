#ifndef _CEK_GPRS_SIM800L_H_
#define _CEK_GPRS_SIM800L_H_

// Select your modem:
#define TINY_GSM_MODEM_SIM800

#include <TinyGsmClient.h>

void initGPRS();
void gprsLoop();

TinyGsmClient& getGsmClient();

#endif