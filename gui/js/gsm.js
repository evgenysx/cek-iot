/**
 * Вкладка GSM
 */

import { onclick, changeValue, getValue } from "./dom";
import {notifyToggleCheckbox, regOnMessage, regOnOpen, requestUpdate, wsEvents} from './ws'

const parseRegStatus = (status) => {
    if (status == -1)
        return "Нет связи с модемом";
    if (status == 0)
        return "Не найдена gsm сеть";
    if (status == 1)
        return "Зарегистрирована";
    if (status == 2)
        return "Поиск сети";
    if (status == 3)
        return "Отказ регистрации в сети";
    if (status == 4)
        return "Обновление статуса";
}

onclick('#gsm_gprs_enabled', notifyToggleCheckbox);
onclick('#gsm_restart_btn', () => {
    console.log("restart gsm modem");
    requestUpdate(wsEvents.gsm.RestartModem);
});

onclick('#gsm_sendsms_btn', () => {
    const phone = getValue('#txt_pnone_sms');
    if (!phone || phone.length != 12){
        console.log("bad phone");
        return;
    }
    const txtSms = getValue('#txt_text_sms');
    if (!txtSms || txtSms.length > 70){
        console.log("bad sms text");
        return;
    }
    requestUpdate(wsEvents.gsm.SendSMS, {to: phone, text: txtSms});
});


regOnMessage(wsEvents.gsm.UpdateStatus, (msg) => {
    console.log('gsm_status changed ' + msg);
    //changeValue("#gsm_status", msg);
});

regOnMessage(wsEvents.gsm.UpdateBattPercent, (msg) => {
    console.log('GsmUpdateBattPercent ' + msg);
    changeValue("#gsm_battery", "Заряд батареи " + msg + "%");
});


regOnMessage(wsEvents.gsm.NetworkInfo, (msg) => {
    let netInfo = JSON.parse(msg);
    console.log(netInfo)
    var info = "Поиск сети ...";
    info = parseRegStatus(netInfo.reg);

    if (netInfo.network){        
        info += " / " + netInfo.operator + " / сигнал " + netInfo.signal;
    }
    changeValue("#gsm_status", info);
});

regOnMessage(wsEvents.gsm.UpdateSignalQuality, (msg) => {
    console.log('UpdateSignalQuality ' + msg);
});

regOnMessage(wsEvents.gsm.GsmGetLocation, (msg) => {
    console.log('GsmGetLocation ' + msg);
});

regOnMessage(wsEvents.gsm.UpdateBalance, (msg) => {
    console.log('UpdateBalance ' + msg);
});

regOnOpen(() => {
    requestUpdate(wsEvents.gsm.UpdateBattPercent);
    //requestUpdate(wsEvents.gsm.UpdateStatus);
    requestUpdate(wsEvents.gsm.NetworkInfo);
    //requestUpdate(wsEvents.gsm.GsmGetLocation);
    //requestUpdate(wsEvents.gsm.UpdateBalance);
    //requestUpdate(wsEvents.gsm.SendSMS);
});
