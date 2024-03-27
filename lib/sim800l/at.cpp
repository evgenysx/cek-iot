#include "at.h"
#include <functional>


ATResponse::ATResponse()
{
  code = eATcode::Unknown;
}

bool ATResponse::isOK()
{
    return true;
}

void BufAtCmd::reserve(uint cap)
{
    this->cap = cap;
    buf = new char[cap];
    memset(buf, 0, cap);
}

bool BufAtCmd::addChar(char c)
{
    // при отсутствии соединения - 0x00
    if(c == 0x0)
       return false; 
    // remove repeat '\r'  
    if(c == '\r'){
        if(buf[len-1] == '\r') return false;
    }
    buf[len] = c;
    len++;
    /**
     * Проверяем окончание выражения
    */
    if(!memcmp(buf+len-2, GSM_NL, 2)){
        // удаляем GSM_NL
        memset(buf+len-2, 0, 2);
        len-=2;    
        if (len == 0)
            return false;
        //

        auto bEnd = parseCmd();
        if(!bEnd){
            // очистка буфера
            memset(buf, 0, len);
            len = 0;
        }
        return bEnd;
    }else if(!memcmp(buf+len-2, SMS_NL, 2)){
        at.response = buf;
        return true;
    }

    return false;
}

bool BufAtCmd::parseCmd()
{
    // Во-первых проверяем, что пришел ответ на команду AT
    if(!memcmp(buf, "AT", 2)){
        // дополнительно - оставляем начало ответа "AT..."
        at.cmd = buf;
        foundAT = true;             
        return false;
    }else if(!memcmp(buf, "OK", 2)){
        // проверка окончания выражения
        at.code = eATcode::Success;
        return true;
    }else{
        at.response += buf;
        // это либо 1) ответ на AT-команд
        // тогда еще ждем 'OK'
        if(foundAT)
            return false;
        //2) URC(незапрашиваемое уведомление)
        at.code = eATcode::Success;
        // бывают сложные команды, например +CDS: 25\r\n....
        if(!memcmp(buf, "+CDS", 4)){
            return false;
        }
        return true;
    }
}

char *BufAtCmd::data()
{
    return buf;
}

void BufAtCmd::clear()
{
  memset(buf, 0, len);
  len = 0;
  foundAT = false;
  at.code = eATcode::Unknown;
  at.cmd = "";
  at.response = "";
}

BufAtCmd::BufAtCmd()
{
  cap = 0;
  len = 0;
  foundAT = false;
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
            if(bufAt.at.response.length() > 0){
                parseCmd(bufAt.at);
            }
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
    sendAT(cmd,0);
}

void ATStream::sendAT(String cmd, String &data)
{
    sendAT(cmd,0);
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
    sendAT("+CIMI");
}

void ATStream::updateSignalQuality()
{
    sendAT("+CSQ");
}

