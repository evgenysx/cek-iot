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
// приглашение на ввод смс PDU
#define SMS_NL "> "


#include <Arduino.h>
#include <map>

enum class eATcode {
  Unknown = -1,
  Success,
  Error
};

/**
 * AT запрос-ответ от модема
*/
struct ATResponse {
    eATcode code;
    // команда
    String cmd;
    // результат ответа от SIM
    String response;

    ATResponse();

    bool isOK();
};


/**
 * Буфер для команды в модем
*/
struct BufAtCmd {
    friend class ATStream;
    private:
        char* buf;
        int len;
        int cap;

        /**
         * at-обмен с модемом
        */
        ATResponse at;

        boolean foundAT;
    public:
        void reserve(uint cap);
        /**
         * добавляет 1 символ из потока
        */
        bool addChar(char c);

        bool parseCmd();

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
    
    virtual bool parseCmd(const ATResponse& at) = 0;
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