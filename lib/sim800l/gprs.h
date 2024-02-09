#ifndef _CEK_GPRS_SIM800L_H_
#define _CEK_GPRS_SIM800L_H_

#include "at.h"

//
enum class eGsmOperator {
    NotSelected = -1, Tele2, Yota, Beeline, MTC, MegaFon
};

static inline String TinyGsmDecodeHex8bit(String& instr) {
  String result;
  for (uint16_t i = 0; i < instr.length(); i += 2) {
    char buf[4] = {
        0,
    };
    buf[0] = instr[i];
    buf[1] = instr[i + 1];
    char b = strtol(buf, NULL, 16);
    result += b;
  }
  return result;
}


class GsmCustomClient : public ATStream{
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


    String getBattVoltage();
    String getGsmLocation();
    
private:
    GsmCustomClient(HardwareSerial& stream);
    
    
    // sms helpers
    void getPDUPack(String *phone, String *message, String *result, int *PDUlen);
    String getDAfield(String *phone, bool fullnum);


    //TinyGsmClient client;

    const String getAPN();

private:   
    // Tele2 / Yota / ...
    eGsmOperator typeOperator;
    RegStatus gsmRegStatus;
};

#endif