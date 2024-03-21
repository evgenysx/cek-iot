#ifndef _CEK_GPRS_SIM800L_H_
#define _CEK_GPRS_SIM800L_H_

#include "at.h"
#include <queue>

/**
 * Список известных смс операторов
*/
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

/**
 * Отчет о доставке смс
 */
struct SmsReportDelivery
{
    String deliveryDate;
    String status;
    String mr;
};

/**
 * Задание на отправку смс 
*/
struct SmsInfo {
    String phone;
    String msg;
    // представление смс в pdu формате
    String pduPack;

    SmsInfo(String& phone, String& msg);
};

/**
 * Класс для работы с смс-модулем
*/
class GsmCustomClient : public ATStream{
public:
    void initGPRS();
    void gprsLoop();
    void taskLoop();
    void getUSSD(const String& code);

    static GsmCustomClient* create(HardwareSerial& serial);

    /**
     * Устройство отвечает на запросы (подключено), но, возможно, еще не сети 
    */
    bool isDeviceConnected();
    /**
     * устройство в домашней сети и готово к работе
    */
    bool isDeviceReady();
    // определение Оператора связи
    void updateOperatorIMSI();
    const String getOperatorName();
    /**
     * перезагружает устройство
    */
    bool restart();
    /**
     * Начальная инициализация устройства
    */
    bool init();

    /**
     * Запуск
    */
    bool start();
    /**
     * Получаем текущий статус регистрации в сети
    */
    const RegStatus getRegStatus();
    /**
     * Добавляем смс в очередь для отправки
    */
    int registerSms(String phone, String message);

    void setRegStatus(RegStatus status);
    void setOperator(eGsmOperator type);

    /**
     * Разбирает ответ смс-модуля(sim800l) на запрос
    */
    virtual bool parseCmd(const ATResponse& at) override;
    String getBattVoltage();
    /**
     * Получение текущих координат Базовой Станции
    */
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
    int sendSMSinPDU(SmsInfo& sms);

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
    void _OnUpdateIMSI(const String& imsi);
    /**
     * Парсинг отчета об отправке смс
    */
    void _OnSmsDeliveryReport(const String& pdu);
    OnUserDataCallback _OnUserSmsDeliveryReport;
    //
    void _OnSmsSent(String&& mr);
    OnUserStrCallback _OnUserSmsSent;
    //
    const String getAPN();

    /**
     * блокирует отправку новых смс.
     * смс можно отправлять только последовательно по 1.
    */
    void lockSmsSend();
    /**
     * разрешает отправку новых смс
    */
    void unLockSmsSend();
private:   
    // Оператор сим-карты Tele2 / Yota / ...
    eGsmOperator typeOperator;
    /**
     * Текущий статуцс регистрации в сети
    */
    RegStatus gsmRegStatus;

    /**
     * Очередь смс на отправку
    */
    std::queue<SmsInfo> smsQueue;
    /**
     * возможность отправки новых смс
    */
    boolean bCanSendSms;
};

#endif