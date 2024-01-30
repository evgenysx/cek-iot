#include "gprs.h"
#include "encode_str.hpp"

// See
//https://codius.ru/articles/GSM_%D0%BC%D0%BE%D0%B4%D1%83%D0%BB%D1%8C_SIM800L_%D1%87%D0%B0%D1%81%D1%82%D1%8C_3


// Set serial for debug console (to the Serial Monitor, default speed 115200)
#define SerialMon Serial
// Define the serial console for debug prints, if needed
#define TINY_GSM_DEBUG SerialMon

GsmCustomClient::GsmCustomClient(Stream& stream) : TinyGsm(stream){
  client.init(this);
  gsmRegStatus = RegStatus::REG_NO_RESULT;
}

void GsmCustomClient::setOperator(eGsmOperator type)
{
  typeOperator = type;
}

const String GsmCustomClient::getOperatorName()
{
  if (typeOperator == eGsmOperator::Tele2)
  {
    return "Tele2";
  }
  else if (typeOperator == eGsmOperator::Yota)
  {
    return "Yota (Скартел)";
  }else{
    return "Незвестная сеть " + String((int)typeOperator);
  }
}

const String GsmCustomClient::getAPN()
{
  if (typeOperator == eGsmOperator::Tele2){
    return "internet.tele2.ru";
  }else if (typeOperator == eGsmOperator::Yota){
    return "internet.yota";
  }
  return "not selected APN";
}

// https://ru.wikipedia.org/wiki/IMSI
void GsmCustomClient::detectOperatorIMSI(){
  String imsi = getIMSI();
  // Mobile Network Code
  const String mnc = imsi.substring(3,5);
  if (mnc.equals("11")){
    setOperator(eGsmOperator::Yota);
  }else if(mnc.equals("20")){
    setOperator(eGsmOperator::Tele2);
  }else if(mnc.equals("01")){
    setOperator(eGsmOperator::MTC);
  }else{
    setOperator(eGsmOperator::NotSelected);
  }

}

void GsmCustomClient::initGPRS() {
  delay(500);
  // Restart takes quite some time
  // To skip it, call init() instead of restart()
  SerialMon.println("Initializing modem...");
  restart();
  // init();
  //setBaud(9600);
  String modemInfo = getModemInfo();
  SerialMon.print("Modem Info: ");
  SerialMon.println(modemInfo);

  SerialMon.print("Waiting for network...");
  if (!waitForNetwork()) {
    SerialMon.println(" fail");
    delay(10000);
    return;
  }
  SerialMon.println(" success");
  //
  
  
  if (isNetworkConnected()) {
    SerialMon.println("Network connected ...");
    SerialMon.println("Welcome to " + getOperatorName() + ". Signal quality = " + getSignalQuality());
  }

  auto apn = getAPN();
  //GPRS connection parameters are usually set after network registration
  SerialMon.print("Connecting to " + apn);
  if (!gprsConnect(apn.c_str(), "", "")) {
    SerialMon.println(" fail");
    delay(10000);
    return;
  }
  SerialMon.println(" success");

  if (isGprsConnected()) { SerialMon.println("GPRS connected"); }
}

void GsmCustomClient::gprsLoop() {
  // Make sure we're still registered on the network
  if (!isNetworkConnected()) {
    SerialMon.println("Network disconnected");
    if (!waitForNetwork(180000L, true)) {
      SerialMon.println(" fail");
      delay(10000);
      return;
    }
    if (isNetworkConnected()) {
      SerialMon.println("Network re-connected");
    }
    // and make sure GPRS/EPS is still connected
    if (!isGprsConnected()) {
      auto apn = getAPN();
      SerialMon.println("GPRS disconnected!");
      SerialMon.print("Connecting to " + apn);
      if (!gprsConnect(apn.c_str(), "", "")) {
        SerialMon.println(" fail");
        delay(10000);
        return;
      }
      if (isGprsConnected()) { SerialMon.println("GPRS reconnected"); }
    }
  }
}


String GsmCustomClient::getUSSD(const String& code)
{
   //return sendUSSD(code);
   String format = "GSM";

    sendAT(GF("+CMGF=1"));
    waitResponse();
    sendAT(GF("+CSCS=\""), format, GF("\""));
    waitResponse();
    sendAT(GF("+CUSD=1,\""), code, GF("\""));
    if (waitResponse() != 1) {
      return "";
    }
    if (waitResponse(10000L, GF(GSM_NL "+CUSD:")) != 1) {
      return "";
    }
    stream.readStringUntil('"');
   String hex = stream.readStringUntil('"');
   stream.readStringUntil(',');
   int dcs = stream.readStringUntil('\n').toInt();

   if (dcs == 15 && format == "HEX") {
     return TinyGsmDecodeHex8bit(hex);
   } else if (dcs == 72 && format == "HEX") {
     return TinyGsmDecodeHex16bit(hex);
   } else {
     return UCS2ToString(hex);
   }
}

int GsmCustomClient::sendSMSinPDU(String phone, String message)
{
  // ============ Подготовка PDU-пакета =============================================================================================
  // В целях экономии памяти будем использовать указатели и ссылки
  String *ptrphone = &phone;                                    // Указатель на переменную с телефонным номером
  String *ptrmessage = &message;                                // Указатель на переменную с сообщением

  String PDUPack;                                               // Переменная для хранения PDU-пакета
  String *ptrPDUPack = &PDUPack;                                // Создаем указатель на переменную с PDU-пакетом

  int PDUlen = 0;                                               // Переменная для хранения длины PDU-пакета без SCA
  int *ptrPDUlen = &PDUlen;                                     // Указатель на переменную для хранения длины PDU-пакета без SCA

  getPDUPack(ptrphone, ptrmessage, ptrPDUPack, ptrPDUlen);      // Функция формирующая PDU-пакет, и вычисляющая длину пакета без SCA

  // Serial.println("PDU-pack: " + PDUPack);
  // Serial.println("PDU length without SCA:" + (String)PDUlen);

  // ============ Отправка PDU-сообщения ============================================================================================
  sendAT(GF("+CMGF=0"));
  if (1 != waitResponse())
    return 1;
  // https://wiki.iarduino.ru/page/a6_gprs_at/#AT_CMGS
  sendAT(GF("+CMGS=" + (String)PDUlen));
  if (1 != waitResponse(GSM_NL)){
    stream.write(0x1B);  // 0x1B  - сообщение не будет отправлено
    stream.flush();
    return 2;
  }
  stream.print(PDUPack);  // Actually send the message
  stream.write(0x1A);  // Terminate the message
  stream.flush();
  // отправка может идти долго
  if (1 != waitResponse(10000))
    return 3;
  
  return 0;
}

bool GsmCustomClient::restart()
{
  bool bRestart = TinyGsm::restart();
  setRegStatus(RegStatus::REG_NO_RESULT);
  setOperator(eGsmOperator::NotSelected);
  return bRestart;
}

const RegStatus GsmCustomClient::getRegStatus()
{
    return gsmRegStatus;
}

void GsmCustomClient::setRegStatus(RegStatus status)
{
  gsmRegStatus = status;
}

void GsmCustomClient::getPDUPack(String *phone, String *message, String *result, int *PDUlen)
{
  // Поле SCA добавим в самом конце, после расчета длины PDU-пакета
  *result += "01";                                // Поле PDU-type - байт 00000001b
  *result += "00";                                // Поле MR (Message Reference)
  *result += getDAfield(phone, true);             // Поле DA
  *result += "00";                                // Поле PID (Protocol Identifier)
  *result += "08";                                // Поле DCS (Data Coding Scheme)
  //*result += "";                                // Поле VP (Validity Period) - не используется

  String msg = StringToUCS2(*message);            // Конвертируем строку в UCS2-формат

  *result += byteToHexString(msg.length() / 2);   // Поле UDL (User Data Length). Делим на 2, так как в UCS2-строке каждый закодированный символ представлен 2 байтами.
  *result += msg;

  *PDUlen = (*result).length() / 2;               // Получаем длину PDU-пакета без поля SCA
  *result = "00" + *result;                       // Добавляем поле SCA
}

String GsmCustomClient::getDAfield(String *phone, bool fullnum) {
  String result = "";
  for (int i = 0; i <= (*phone).length(); i++) {  // Оставляем только цифры
    if (isDigit((*phone)[i])) {
      result += (*phone)[i];
    }
  }
  int phonelen = result.length();                 // Количество цифр в телефоне
  if (phonelen % 2 != 0) result += "F";           // Если количество цифр нечетное, добавляем F

  for (int i = 0; i < result.length(); i += 2) {  // Попарно переставляем символы в номере
    char symbol = result[i + 1];
    result = result.substring(0, i + 1) + result.substring(i + 2);
    result = result.substring(0, i) + (String)symbol + result.substring(i);
  }

  result = fullnum ? "91" + result : "81" + result; // Добавляем формат номера получателя, поле PR
  result = byteToHexString(phonelen) + result;    // Добавляем длиу номера, поле PL

  return result;
}

GsmCustomClient *GsmCustomClient::create(HardwareSerial& serial)
{
   auto gsmClient = new GsmCustomClient(serial);
   serial.begin(9600);
   return gsmClient;
}

bool GsmCustomClient::isDeviceConnected()
{
    return testAT(100);
}
