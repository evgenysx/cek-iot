#ifndef _CEK_GPRS_SIM800L_H_
#define _CEK_GPRS_SIM800L_H_

#include "at.h"
#include <queue>
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

typedef std::function<void(RegStatus)> OnUserRegStatus;
typedef std::function<void(String)> OnUserStrCallback;

typedef std::function<void(String&, String&)> OnUserStr2Callback;
typedef std::function<void(void*)> OnUserDataCallback;

struct SmsReportDelivery
{
    String deliveryDate;
    String status;
    String mr;
};

struct SmsInfo {
    String phone;
    String msg;

    SmsInfo(String& phone, String& msg);
};

class GsmCustomClient : public ATStream{
public:
    void initGPRS();
    void gprsLoop();
    void taskLoop();
    void getUSSD(const String& code);

    static GsmCustomClient* create(HardwareSerial& serial);

    bool isDeviceConnected();
    // определение Оператора связи
    void updateOperatorIMSI();
    const String getOperatorName();
    bool restart();

    const RegStatus getRegStatus();
    int registerSms(String phone, String message);

    void setRegStatus(RegStatus status);
    void setOperator(eGsmOperator type);


    virtual bool parseCmd(char* cmd) override;
    String getBattVoltage();
    String getGsmLocation();

    void setOnUserRegStatus(OnUserRegStatus callback);
    void setOnUserSignalQuality(OnUserStrCallback callback);
    void setOnUserBalanceUpdate(OnUserStrCallback callback);
    void setOnUserNetworkUpdate(OnUserStr2Callback callback);
    void setOnUserSmsDeliveryReport(OnUserDataCallback callback);
    void setOnUserSmsSent(OnUserStrCallback callback);

    void updateRegistrationStatus();
    void updateBalance();
private:
    GsmCustomClient(HardwareSerial& stream);
    
    
    // sms helpers
    void getPDUPack(String *phone, String *message, String *result, int *PDUlen);
    String getDAfield(String *phone, bool fullnum);
    int sendSMSinPDU(String phone, String message);

    //
    void _OnRegStatus(String&& status);
    OnUserRegStatus _OnUserRegStatus;

    void _OnSignalQuality(String&& data);
    OnUserStrCallback _OnUserSignalQuality;
    //
    void _OnBalanceUpdate(String& data, int dcs);
    OnUserStrCallback _OnUserBalanceUpdate;
    //
    void _OnNetworkInfoUpdate(String&& key, String& value);
    OnUserStr2Callback _OnUserNetworkInfoUpdate;
    //
    void _OnUpdateIMSI(String& imsi);
    /**
     * Парсинг отчета об отправке смс
    */
    void _OnSmsDeliveryReport(String& pdu);
    OnUserDataCallback _OnUserSmsDeliveryReport;
    //
    void _OnSmsSent(String&& mr);
    OnUserStrCallback _OnUserSmsSent;
    //
    const String getAPN();

private:   
    // Tele2 / Yota / ...
    eGsmOperator typeOperator;
    RegStatus gsmRegStatus;
    int8_t iRegStatusReq;

    std::queue<SmsInfo> smsQueue;
    boolean bCanSendSms;
};

#endif