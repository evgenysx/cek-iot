#include "at.h"
#include <functional>


ATResponse::ATResponse()
{
  code = 0;
  isFill = false;
}

bool ATResponse::isOK()
{
    return code == 1;
}

void BufAtCmd::reserve(uint cap)
{
    this->cap = cap;
    buf = new char[cap];
    buf[0] = 0x0;
}

bool BufAtCmd::addChar(char c)
{
    if(c == 0x0)
      return false;
    buf[len] = c;
    len++;

    if(!memcmp(buf+len-2, GSM_NL, 2)){
      buf[len-2] = 0x0;
      len-=2;
      return len > 0;
    }
    
    return false;
}

char *BufAtCmd::data()
{
    return buf;
}

void BufAtCmd::clear()
{
  buf[0] = 0x0;
  len = 0;
}

BufAtCmd::BufAtCmd()
{
  cap = 0;
  len = 0;
}

void ATStream::onReceive()
{
    int iCount = stream.available();
    Serial.println("onReceive isFill=" + String(atResponse.isFill) + " / icount= "+ String(iCount));
    long _timeout = millis() + 10000;             // Переменная для отслеживания таймаута
    while (atResponse.isFill && millis() < _timeout)  {}; 

    
    for (int i = 0; i < iCount; i++)
    {
        char c = (char)stream.read();
        // накапливаем ответ в буфере
        bool bReady = bufAt.addChar(c);
        if (bReady)
        {
            parseCmd(bufAt.data());
            bufAt.clear();
        }
    }
}

inline void ATStream::waitResponse(uint32_t timeout_ms)
{
    long _timeout = millis() + timeout_ms;             // Переменная для отслеживания таймаута
    while (!atResponse.isFill && millis() < _timeout)  {}; // Ждем ответа 10 секунд, если пришел ответ или наступил таймаут, то...
}

inline void ATStream::waitResponse()
{
    waitResponse(1000);
}

/**
 * Разбирает законченное выражение от sim-модуля
*/
void ATStream::parseCmd(char *cmd)
{
    Serial.println("Parsing: " + String(cmd) + " / isFill=" + String(atResponse.isFill));
    if (!memcmp(cmd, "+CREG", 5)){
        auto scmd = String(cmd);
        
        atResponse.code = 1;
        atResponse.param[0] = scmd.substring(7,8);
        atResponse.param[1] = scmd.substring(9,10);
        Serial.println("Parsed CREG = " + atResponse.param[0] + " / " + atResponse.param[1]);
    }else if (!memcmp(cmd, "+CSQ", 4)){
        auto scmd = String(cmd);
        
        atResponse.code = 1;
        int delim = scmd.indexOf(',');
        atResponse.param[0] = scmd.substring(6,delim);
        atResponse.param[1] = scmd.substring(delim+1,1);
        Serial.println("Parsed CSQ = " + String(atResponse.param[0]));
    }else if(!memcmp(cmd, "+CME ERROR", 10)){
        return;
    }else if(!memcmp(cmd, "OK", 2)){
        atResponse.code = 1;
        return;
    }else if(!memcmp(cmd, "ERROR", 5)){
        atResponse.code = 0;
        return;
    }else{
        atResponse.code = 1;
        atResponse.param[0] = cmd;      
    }
    atResponse.isFill = true;
}

ATStream::ATStream(HardwareSerial &stream)
    : stream(stream)
{
    bufAt.reserve(64);
    stream.onReceive(std::bind(&ATStream::onReceive, this));
}

ATResponse ATStream::sendAT(String cmd)
{
    return sendAT(cmd,1000);
}

ATResponse ATStream::sendAT(String cmd, String &data)
{
    return sendAT(cmd,1000);
}

ATResponse ATStream::sendAT(String cmd, uint32_t timeout_ms)
{
    const String at = "AT" + cmd + GSM_NL;
    stream.print(at.c_str());
    stream.flush();
    //
    //waitResponse(timeout_ms);
    return atResponse;
}

String ATStream::getIMSI()
{
    Serial.println("send IMSI");
    auto resp = sendAT(GF("+CIMI"), 3000);
    resp.isFill = false;
    Serial.println("getIMSI = " + resp.param[0]);
    return resp.param[0];
}

String ATStream::getSignalQuality()
{
    auto resp = sendAT(GF("+CSQ"));
    resp.isFill = false;
    return resp.param[0];
}

RegStatus ATStream::getRegistrationStatus()
{
    Serial.println("send CREG");
    sendAT("+CREG?", 2000);
    if (atResponse.code != 1)
        return RegStatus::REG_NO_RESULT;
    auto resp = (RegStatus)atoi(atResponse.param[1].c_str());
    Serial.println("GetRegistrationStatus = " + String(resp));
    atResponse.isFill = false;
    return resp;
}
