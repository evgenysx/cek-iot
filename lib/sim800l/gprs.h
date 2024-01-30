#ifndef _CEK_GPRS_SIM800L_H_
#define _CEK_GPRS_SIM800L_H_

// Select your modem:
#define TINY_GSM_MODEM_SIM800

#include <TinyGsmClient.h>

enum class eGsmOperator {
    NotSelected = -1, Tele2, Yota, Beeline, MTC, MegaFon
};

class GsmCustomClient : public TinyGsm{
public:
    void initGPRS();
    void gprsLoop();
    String getUSSD(const String& code);

    static GsmCustomClient* create(HardwareSerial& serial);

    bool isDeviceConnected();
    // определение Оператора связи
    void detectOperatorIMSI();
    const String getOperatorName();
    int sendSMSinPDU(String phone, String message);
    bool restart();

    const RegStatus getRegStatus();
    void setRegStatus(RegStatus status);
    void setOperator(eGsmOperator type);

private:
    GsmCustomClient(Stream& stream);
    
    
    // sms helpers
    void getPDUPack(String *phone, String *message, String *result, int *PDUlen);
    String getDAfield(String *phone, bool fullnum);
    // Tele2 / Yota / ...
    eGsmOperator typeOperator;
    TinyGsmClient client;

    const String getAPN();

    RegStatus gsmRegStatus;
};

#endif