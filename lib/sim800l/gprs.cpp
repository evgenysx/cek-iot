#include "gprs.h"
#include "encode_str.hpp"


// Set serial for debug console (to the Serial Monitor, default speed 115200)
#define SerialMon Serial
// Define the serial console for debug prints, if needed
#define TINY_GSM_DEBUG SerialMon

GsmCustomClient::GsmCustomClient(Stream& stream) : TinyGsm(stream){
  client.init(this);
}

void GsmCustomClient::setOperator(eGsmOperator type)
{
  typeOperator = type;
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

  if (isNetworkConnected()) { SerialMon.println("Network connected"); }


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


String GsmCustomClient::getBalance(const String& code)
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

GsmCustomClient *GsmCustomClient::create(HardwareSerial& serial, eGsmOperator type)
{
   auto gsmClient = new GsmCustomClient(serial);
   gsmClient->setOperator(type);
   serial.begin(9600);
   return gsmClient;
}
