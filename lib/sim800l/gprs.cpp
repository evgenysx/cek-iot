#include "gprs.h"
#include "encode_str.h"

// See
//https://codius.ru/articles/GSM_%D0%BC%D0%BE%D0%B4%D1%83%D0%BB%D1%8C_SIM800L_%D1%87%D0%B0%D1%81%D1%82%D1%8C_3


// Set serial for debug console (to the Serial Monitor, default speed 115200)
#define SerialMon Serial
// Define the serial console for debug prints, if needed
#define TINY_GSM_DEBUG SerialMon

/**
 * Время последней попытки соединения с смс модулем(sim800l)
*/
unsigned long timeTryConnect = 0;

GsmCustomClient::GsmCustomClient(HardwareSerial &stream)
    : ATStream(stream)
{
    init();
    // возможность отправки новых смс
    bCanSendSms = true;
}

void GsmCustomClient::setOperator(eGsmOperator type)
{
  typeOperator = type;
}

bool GsmCustomClient::parseCmd(const ATResponse& at)
{
  
  Serial.println("parseCmd " + at.cmd + " / resp = " + at.response);
  //link
  auto& r = at.response;
  if (r.startsWith("+CREG"))
  {
    // ответ может прийти в 2 форматах
    // 1. +CREG: 1,1  - ответ в явном виде, когда был запрос CREG?
    // 2. +CREG: 1 - уведомление в неявном виде, при смене статуса
    if(r.length() > 8)
      _OnRegStatus(r.substring(9, 10)); // +CREG: 1,1 - вторая цифра
    else
      _OnRegStatus(r.substring(7,8));
  }
  else if (r.startsWith("+CSQ"))
  {
    int delim = r.indexOf(',');
    r.substring(delim + 1, 1);
    _OnSignalQuality(r.substring(6, delim));
  }
  else if (at.cmd.startsWith("AT+CMGF"))
  {
    
  }
  else if (r.startsWith("+CDS"))
  {
    // отчет об доставке СМС
    // example
    // +CDS: 2507919712690080F806040B919780618043F8423002718352214230027183722100
    _OnSmsDeliveryReport(r.substring(8));
  }
  else if (r.startsWith("+CME ERROR"))
  {
    
  }
  else if (r.startsWith("OK"))
  {
    
  }
  else if (r.startsWith("AT+CFUN=0"))
  {
    init();
    // пауза перед включением
    delay(3000);
    //включаем устройство
    sendAT("+CFUN=1,1");
    delay(3000);
  }
  else if (r.startsWith("ERROR"))
  {
  }
  else if (r.startsWith("+CUSD"))
  {
     int delim = r.indexOf('"');
     int delimEnd = r.indexOf('"',delim+1);

    auto hex = r.substring(delim+1, delimEnd);
    auto dcs = atoi(r.substring(delimEnd + 2).c_str());

    _OnBalanceUpdate(hex, dcs);
  }else if (r.startsWith("+CMGS"))
  {
    _OnSmsSent(r.substring(7));
  }
  else if (r.startsWith("+"))
  {
    Serial.println("income " + r);
  }
  else if (r.startsWith(SMS_NL))
  {
    // пишем pdu + конец смс char(0x1A)  
    write(smsQueue.front().pduPack + String(char(0x1A)));
  }
  else if (r.startsWith("AT+CIMI"))
  {
    _OnUpdateIMSI(at.response);
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

void GsmCustomClient::_OnUpdateIMSI(const String &imsi)
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

void GsmCustomClient::_OnSmsDeliveryReport(const String &pdu)
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
  unLockSmsSend();
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

void GsmCustomClient::lockSmsSend()
{
  bCanSendSms = false;
}

void GsmCustomClient::unLockSmsSend()
{
  // при успешной отправке смс - удаляем из очереди
  smsQueue.pop();
  // можно отправлять новые смс
  bCanSendSms = true;
}

void GsmCustomClient::updateRegistrationStatus()
{
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
  // задача - отправка смс
   if (!smsQueue.empty() && bCanSendSms){
    auto& sms = smsQueue.front();
    if(!sendSMSinPDU(sms)){
      ;
    }
   }
   // задача - проверка подключения
   if(!isDeviceConnected()){
    // попытка начать работу с модемом
    if (millis() - timeTryConnect < 5000)
      return;

    Serial.println("Try to connect sim800l ");

    timeTryConnect = millis();
    start();
   }
}

void GsmCustomClient::getUSSD(const String& code)
{
  String format = "GSM";
  sendAT(GF("+CMGF=1"));
  sendAT(String(GF("+CSCS=\"")) + format + GF("\""));
  sendAT(String(GF("+CUSD=1,\"")) + code + GF("\""));
}

int GsmCustomClient::sendSMSinPDU(SmsInfo& sms)
{
  if(!isDeviceReady())
    return -1;
  // блокируем отправку сразу нескольких смс  
  lockSmsSend();
  // 
  // ============ Подготовка PDU-пакета =============================================================================================
  // В целях экономии памяти будем использовать указатели и ссылки
  String *ptrphone = &sms.phone;                                    // Указатель на переменную с телефонным номером
  String *ptrmessage = &sms.msg;                                // Указатель на переменную с сообщением
                                              // Переменная для хранения PDU-пакета
  String *ptrPDUPack = &sms.pduPack;                                // Создаем указатель на переменную с PDU-пакетом

  int PDUlen = 0;                                               // Переменная для хранения длины PDU-пакета без SCA
  int *ptrPDUlen = &PDUlen;                                     // Указатель на переменную для хранения длины PDU-пакета без SCA

  getPDUPack(ptrphone, ptrmessage, ptrPDUPack, ptrPDUlen);      // Функция формирующая PDU-пакет, и вычисляющая длину пакета без SCA

  //Serial.println("PDU-pack: " + sms.pduPack);
  //Serial.println("PDU length without SCA:" + (String)PDUlen);

  // ============ Отправка PDU-сообщения ============================================================================================
  //sendAT("+CMGF=0");
    // https://wiki.iarduino.ru/page/a6_gprs_at/#AT_CMGS
  sendAT("+CMGS=" + (String)PDUlen, 0);
  //
  //write(PDUPack + String(char(0x1A)));
    return 0;
}

bool GsmCustomClient::restart()
{
  //выключаем устройство
  Serial.println("restart 0");
  sendAT("+CFUN=0");
  return true;
}

bool GsmCustomClient::init()
{
  Serial.println("init gsm");
  setRegStatus(RegStatus::REG_NO_RESULT);
  setOperator(eGsmOperator::NotSelected);
  return true;
}

bool GsmCustomClient::start()
{
    // эхо
    sendAT(GF("E1"));
     
    Serial.println("start gsm routine");
    sendAT(GF("+CREG=1"));
    sendAT("+CMGF?");
    updateRegistrationStatus();
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
  auto status = getRegStatus();
  return ((status != REG_NO_RESULT) && (status != REG_UNKNOWN));
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
