#include "at.h"
#include <functional>


ATResponse::ATResponse()
{
  code = 0;
}

bool ATResponse::isOK()
{
    return code == 1;
}

void BufAtCmd::reserve(uint cap)
{
    this->cap = cap;
    buf = new char[cap];
    memset(buf, 0, len);
}

bool BufAtCmd::addChar(char c)
{
    if(c == 0x0)
      return false;
    buf[len] = c;
    len++;

    if(!memcmp(buf+len-2, GSM_NL, 2)){
        memset(buf+len-2, 0, 2);
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
  memset(buf, 0, len);
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
    //Serial.println("onReceive isFill=" + String(atResponse.isFill) + " / icount= "+ String(iCount));
    
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


ATStream::ATStream(HardwareSerial &stream)
    : stream(stream)
{
    bufAt.reserve(256);
    stream.onReceive(std::bind(&ATStream::onReceive, this));
}

void ATStream::sendAT(String cmd)
{
    sendAT(cmd,1000);
}

void ATStream::sendAT(String cmd, String &data)
{
    sendAT(cmd,1000);
}

void ATStream::sendAT(String cmd, uint32_t timeout_ms)
{
    const String at = "AT" + cmd + GSM_NL;
    stream.print(at.c_str());
    stream.flush();
    delay(timeout_ms);
}

void ATStream::write(String data)
{
    stream.print(data.c_str());
    stream.flush();
    delay(500);
}

void ATStream::updateIMSI()
{
    Serial.println("send IMSI");
    sendAT(GF("+CIMI"));
}

void ATStream::updateSignalQuality()
{
    sendAT(GF("+CSQ"));
}

