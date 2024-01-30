/**
 * Вкладка GSM
 */

import { onclick, changeValue, getValue } from "./dom";
import {notifyToggleCheckbox, regOnMessage, regOnOpen, requestUpdate, wsEvents} from './ws'

// есть ли связь с модемом
let isConnectedGsmModem = -1;
let networkGsmInfo = {};


onclick('#tab_gsm', () => {
    requestUpdate(wsEvents.gsm.GsmEnableUpdateNetworkInfo, {value: true});
});

onclick('#tab_sensor', () => {
    requestUpdate(wsEvents.gsm.GsmEnableUpdateNetworkInfo, {value: false});
});

onclick('#tab_device', () => {
    requestUpdate(wsEvents.gsm.GsmEnableUpdateNetworkInfo, {value: false});
});

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

const updateNetworkInfo = (netInfo) => {
    networkGsmInfo =  {...networkGsmInfo, ...netInfo};
    renderNetworkInfo();
}
const setNetworkInfo = (netInfo) => {
    networkGsmInfo = netInfo;
    renderNetworkInfo();
}

const renderNetworkInfo = () => {
    let txtStatus = "Нет связи с модемом";
    switch (networkGsmInfo.reg){     
        case 0:
            txtStatus = "Не найдена gsm сеть";
            break;
        case 1: 
            txtStatus = networkGsmInfo.operator + " / " + networkGsmInfo.signal;
            break;
        case 2:
            txtStatus = "Поиск сети " + networkGsmInfo.operator + " / " + networkGsmInfo.signal;
            break;
        case 3:
            txtStatus = "Отказ регистрации " + networkGsmInfo.operator + " / " + networkGsmInfo.signal;
            break;
        case 4:
            txtStatus = "Обновление статуса";
            break;
    }
    changeValue("#gsm_status", txtStatus);
}

regOnMessage(wsEvents.gsm.UpdateStatus, (statusGsm) => {
    if (isConnectedGsmModem != statusGsm){
        // появилось соединение с модемом
        if (statusGsm)
            requestUpdate(wsEvents.gsm.NetworkInfo);
        else{
            // пропало соединение
            updateNetworkInfo({"network": false});
        }
        console.log('gsm_status changed to ' + statusGsm); 
    }
});

regOnMessage(wsEvents.gsm.UpdateBattPercent, (msg) => {
    console.log('GsmUpdateBattPercent ' + msg);
    changeValue("#gsm_battery", "Заряд батареи " + msg + "мВ");
});


regOnMessage(wsEvents.gsm.NetworkInfo, (msg) => {
    let netInfo = JSON.parse(msg);
    setNetworkInfo(netInfo);
});

regOnMessage(wsEvents.gsm.UpdateSignalQuality, (signal) => {
    updateNetworkInfo({signal:signal});
});

regOnMessage(wsEvents.gsm.GsmGetLocation, (msg) => {
    console.log('GsmGetLocation ' + msg);
});

regOnMessage(wsEvents.gsm.UpdateBalance, (msg) => {
    console.log('UpdateBalance ' + msg);
});

regOnOpen(() => {
    //requestUpdate(wsEvents.gsm.UpdateStatus);
    requestUpdate(wsEvents.gsm.UpdateBattPercent);
    //requestUpdate(wsEvents.gsm.UpdateStatus);
    requestUpdate(wsEvents.gsm.NetworkInfo);
    requestUpdate(wsEvents.gsm.GsmEnableUpdateNetworkInfo, {value: true});
});
