#include "gprs.h"
#include "encode_str.h"

// See
//https://codius.ru/articles/GSM_%D0%BC%D0%BE%D0%B4%D1%83%D0%BB%D1%8C_SIM800L_%D1%87%D0%B0%D1%81%D1%82%D1%8C_3


// Set serial for debug console (to the Serial Monitor, default speed 115200)
#define SerialMon Serial
// Define the serial console for debug prints, if needed
#define TINY_GSM_DEBUG SerialMon

GsmCustomClient::GsmCustomClient(HardwareSerial &stream)
    : ATStream(stream)
{
    gsmRegStatus = RegStatus::REG_NO_RESULT;
    iRegStatusReq = 0;
    bCanSendSms = true;
}

void GsmCustomClient::setOperator(eGsmOperator type)
{
  typeOperator = type;
}

bool GsmCustomClient::parseCmd(char *cmd)
{
  Serial.println("parseCmd " + String(cmd));
  auto scmd = String(cmd);

  if (!memcmp(cmd, "+CREG", 5))
  {
    

    // scmd.substring(7,8);
    // scmd.substring(9,10);
    _OnRegStatus(scmd.substring(9, 10));
  }
  else if (!memcmp(cmd, "+CSQ", 4))
  {
    int delim = scmd.indexOf(',');
    //auto signal =;
    scmd.substring(delim + 1, 1);
    _OnSignalQuality(scmd.substring(6, delim));
  }
  else if (!memcmp(cmd, "+CMGS", 4))
  {
    // отчет об отправке СМС
    _OnSmsSent(scmd.substring(7));
  }
  else if (!memcmp(cmd, "+CDS", 4))
  {
    // отчет об доставке СМС придет следующим сообщением
  }
  else if (!memcmp(cmd, "+CME ERROR", 10))
  {
    
  }
  else if (!memcmp(cmd, "OK", 2))
  {
   
  }
  else if (!memcmp(cmd, "ERROR", 5))
  {
  }
  else if (!memcmp(cmd, "+USD", 1))
  {
     int delim = scmd.indexOf('"');
     int delimEnd = scmd.indexOf('"',delim+1);

    auto hex = scmd.substring(delim+1, delimEnd);
    auto dcs = atoi(scmd.substring(delimEnd + 2).c_str());

    _OnBalanceUpdate(hex, dcs);
  }
  else if (!memcmp(cmd, "+", 1))
  {
    Serial.println("income " + scmd);
  }
  else if (strlen(cmd) == 15)
  {
    _OnUpdateIMSI(scmd);
  }
  else if (strlen(cmd) == 66)
  {
    // example, 0791198994800721C6220C91197940005637902001917360229020019173602249
    _OnSmsDeliveryReport(scmd);
  }
  else
  {
   
  }
  return true;
}

String GsmCustomClient::getBattVoltage()
{
    return String();
}

String GsmCustomClient::getGsmLocation()
{
    return String();
}

void GsmCustomClient::setOnUserRegStatus(OnUserRegStatus callback)
{
  _OnUserRegStatus = callback;
}

void GsmCustomClient::setOnUserSignalQuality(OnUserStrCallback callback)
{
  _OnUserSignalQuality = callback;
}

void GsmCustomClient::setOnUserBalanceUpdate(OnUserStrCallback callback)
{
  _OnUserBalanceUpdate = callback;
}

void GsmCustomClient::setOnUserNetworkUpdate(OnUserStr2Callback callback)
{
  _OnUserNetworkInfoUpdate = callback;
}

void GsmCustomClient::setOnUserSmsDeliveryReport(OnUserDataCallback callback)
{
  _OnUserSmsDeliveryReport = callback;
}

void GsmCustomClient::setOnUserSmsSent(OnUserStrCallback callback)
{
  _OnUserSmsSent = callback;
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

void GsmCustomClient::_OnRegStatus(String&& status)
{
  iRegStatusReq--;

  auto regStatus = (RegStatus)atoi(status.c_str());
  // обновляем данные об операторе
  if(regStatus == RegStatus::REG_SEARCHING || regStatus == RegStatus::REG_OK_HOME ){
    if (typeOperator == eGsmOperator::NotSelected){
      updateIMSI();
    }else{
      updateSignalQuality();
    }
  }else{
    setOperator(eGsmOperator::NotSelected);
  }
  //
  setRegStatus(regStatus);
  if (_OnUserRegStatus != NULL){
    //_OnUserRegStatus(regStatus);
    _OnNetworkInfoUpdate("reg", status);
  }
}

void GsmCustomClient::_OnSignalQuality(String &&data)
{
  _OnNetworkInfoUpdate("signal", data);
}

void GsmCustomClient::_OnBalanceUpdate(String &data, int dcs)
{
  //http://codius.ru/articles/GSM_%D0%BC%D0%BE%D0%B4%D1%83%D0%BB%D1%8C_SIM800L_%D1%87%D0%B0%D1%81%D1%82%D1%8C_3
 if (_OnUserBalanceUpdate != NULL){
    _OnUserBalanceUpdate(UCS2ToString(data));
  }
}

void GsmCustomClient::_OnNetworkInfoUpdate(String &&key, String &value)
{
   if (_OnUserNetworkInfoUpdate != NULL){
    _OnUserNetworkInfoUpdate(key,value);
  }
}

void GsmCustomClient::_OnUpdateIMSI(String &imsi)
{
  // https://ru.wikipedia.org/wiki/IMSI
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
  auto net = getOperatorName();
  _OnNetworkInfoUpdate("operator", net);
}

void GsmCustomClient::_OnSmsDeliveryReport(String &pdu)
{
  // https://smsconnexion.wordpress.com/2009/02/12/sms-pdu-formats-demystified/
  // 0791198994800721 C6220C91197940005637902001917360229020019173602249
  // 07 Length of the SMSC information.
  // 91 Type of address of the SMSC.
  // ...
  const auto ln = pdu.length(); 
  SmsReportDelivery info;
  info.deliveryDate = revertBytes(pdu.substring(ln-16, ln-4));
  info.status = pdu.substring(ln-2);
  info.mr = pdu.substring(18, 20);

  if (_OnUserSmsDeliveryReport != NULL){
    _OnUserSmsDeliveryReport(&info);
  }
}

void GsmCustomClient::_OnSmsSent(String &&mr)
{
  bCanSendSms = true;
  if (_OnUserSmsSent != NULL){
    _OnUserSmsSent(mr);
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

void GsmCustomClient::updateRegistrationStatus()
{
    if (iRegStatusReq++ > 2){
      setRegStatus(RegStatus::REG_NO_RESULT);
      iRegStatusReq = 0;
    } 
    sendAT(GF("+CREG?"));
}

void GsmCustomClient::updateBalance()
{
  getUSSD("*100#");
}

void GsmCustomClient::initGPRS() {
  // delay(500);
  // // Restart takes quite some time
  // // To skip it, call init() instead of restart()
  // SerialMon.println("Initializing modem...");
  // restart();
  // // init();
  // //setBaud(9600);
  // String modemInfo = getModemInfo();
  // SerialMon.print("Modem Info: ");
  // SerialMon.println(modemInfo);

  // SerialMon.print("Waiting for network...");
  // if (!waitForNetwork()) {
  //   SerialMon.println(" fail");
  //   delay(10000);
  //   return;
  // }
  // SerialMon.println(" success");
  // //
  
  
  // if (isNetworkConnected()) {
  //   SerialMon.println("Network connected ...");
  //   SerialMon.println("Welcome to " + getOperatorName() + ". Signal quality = " + getSignalQuality());
  // }

  // auto apn = getAPN();
  // //GPRS connection parameters are usually set after network registration
  // SerialMon.print("Connecting to " + apn);
  // if (!gprsConnect(apn.c_str(), "", "")) {
  //   SerialMon.println(" fail");
  //   delay(10000);
  //   return;
  // }
  // SerialMon.println(" success");

  // if (isGprsConnected()) { SerialMon.println("GPRS connected"); }
}

void GsmCustomClient::gprsLoop() {
  // // Make sure we're still registered on the network
  // if (!isNetworkConnected()) {
  //   SerialMon.println("Network disconnected");
  //   if (!waitForNetwork(180000L, true)) {
  //     SerialMon.println(" fail");
  //     delay(10000);
  //     return;
  //   }
  //   if (isNetworkConnected()) {
  //     SerialMon.println("Network re-connected");
  //   }
  //   // and make sure GPRS/EPS is still connected
  //   if (!isGprsConnected()) {
  //     auto apn = getAPN();
  //     SerialMon.println("GPRS disconnected!");
  //     SerialMon.print("Connecting to " + apn);
  //     if (!gprsConnect(apn.c_str(), "", "")) {
  //       SerialMon.println(" fail");
  //       delay(10000);
  //       return;
  //     }
  //     if (isGprsConnected()) { SerialMon.println("GPRS reconnected"); }
  //   }
  // }
}

void GsmCustomClient::taskLoop()
{
  // отправка смс
   if (!smsQueue.empty() && bCanSendSms){
    auto& sms = smsQueue.front();
    if(!sendSMSinPDU(sms.phone, sms.msg)){
      // при успешной отправке смс - удаляем из очереди
      smsQueue.pop();
    }
   }
}

void GsmCustomClient::getUSSD(const String& code)
{
  String format = "GSM";
  sendAT(GF("+CMGF=1"));
  sendAT(String(GF("+CSCS=\"")) + format + GF("\""));
  sendAT(String(GF("+CUSD=1,\"")) + code + GF("\""));
}

int GsmCustomClient::sendSMSinPDU(String phone, String message)
{
  if(!isDeviceReady())
    return -1;
  //  
  bCanSendSms = false;  
  // ============ Подготовка PDU-пакета =============================================================================================
  // В целях экономии памяти будем использовать указатели и ссылки
  String *ptrphone = &phone;                                    // Указатель на переменную с телефонным номером
  String *ptrmessage = &message;                                // Указатель на переменную с сообщением

  String PDUPack;                                               // Переменная для хранения PDU-пакета
  String *ptrPDUPack = &PDUPack;                                // Создаем указатель на переменную с PDU-пакетом

  int PDUlen = 0;                                               // Переменная для хранения длины PDU-пакета без SCA
  int *ptrPDUlen = &PDUlen;                                     // Указатель на переменную для хранения длины PDU-пакета без SCA

  getPDUPack(ptrphone, ptrmessage, ptrPDUPack, ptrPDUlen);      // Функция формирующая PDU-пакет, и вычисляющая длину пакета без SCA

  //Serial.println("PDU-pack: " + PDUPack);
  //Serial.println("PDU length without SCA:" + (String)PDUlen);

  // ============ Отправка PDU-сообщения ============================================================================================
  sendAT(GF("+CMGF=0"));
  // https://wiki.iarduino.ru/page/a6_gprs_at/#AT_CMGS
  sendAT("+CMGS=" + (String)PDUlen, 100);
  write(PDUPack + String(char(0x1A)));
  return 0;
}

bool GsmCustomClient::restart()
{
  //bool bRestart = TinyGsm::restart();
  setRegStatus(RegStatus::REG_NO_RESULT);
  setOperator(eGsmOperator::NotSelected);
  return true;
}

const RegStatus GsmCustomClient::getRegStatus()
{
    return gsmRegStatus;
}

int GsmCustomClient::registerSms(String phone, String message)
{
    smsQueue.push(SmsInfo(phone, message));
    Serial.println("registerSms / size = " + String(smsQueue.size()));
    return 0;
}

void GsmCustomClient::setRegStatus(RegStatus status)
{
  gsmRegStatus = status;
}

void GsmCustomClient::getPDUPack(String *phone, String *message, String *result, int *PDUlen)
{
  // Поле SCA добавим в самом конце, после расчета длины PDU-пакета
  *result += "21";                                // Поле PDU-type - байт 00000001b
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
    return (getRegStatus() != REG_NO_RESULT);
}

bool GsmCustomClient::isDeviceReady()
{
    return (getRegStatus() == REG_OK_HOME);
}

SmsInfo::SmsInfo(String &phone, String &msg)
{
  this->phone = phone;
  this->msg = msg;
}
