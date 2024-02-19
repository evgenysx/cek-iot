#ifndef _CEK_AT_SIM800L_H_
#define _CEK_AT_SIM800L_H_


/*
    из заголовка мне нужны только типы данных
*/
//=============================================
#include "TinyGsmClientSIM800.h"
//=============================================
// Select your modem:
#define TINY_GSM_MODEM_SIM800

#include <Arduino.h>
#include <map>
struct ATResponse {
    // результат ответа от SIM
    uint8_t code;
    String param[5];

    ATResponse();

    bool isOK();
};


struct BufAtCmd {
    private:
        char* buf;
        int len;
        int cap;
    public:
        void reserve(uint cap);
        
        bool addChar(char c);

        char* data();
        void clear();
        BufAtCmd & operator +=(const char *cstr);
        BufAtCmd();
};


class ATStream {
private:  
    HardwareSerial& stream;
    ATResponse atResponse;
    BufAtCmd bufAt;


private:
    void onReceive();
    inline void waitResponse(uint32_t timeout_ms);
    inline void waitResponse();

public:
    
    virtual bool parseCmd(char* cmd) = 0;
    ATStream(HardwareSerial &stream);
    void sendAT(String cmd);
    void sendAT(String cmd, String& data);
    void sendAT(String cmd, uint32_t timeout_ms);
    void write(String data);

public:
    void updateIMSI();
    void updateSignalQuality();
};

#endif