/**
 * Вкладка GSM
 */

import { onclick, changeValue, getValue } from "./dom";
import {notifyToggleCheckbox, regOnMessage, regOnOpen, requestUpdate, wsEvents} from './ws'
import {openModal} from './modal'

// информация о состоянии сети
let networkGsmInfo = {};


onclick('#tab_gsm', () => {
    setNetworkUpdateInfo(true);
});

onclick('#tab_sensor', () => {
    setNetworkUpdateInfo(false);
});

onclick('#tab_device', () => {
    setNetworkUpdateInfo(false);
});

onclick('#gsm_gprs_enabled', notifyToggleCheckbox);
onclick('#gsm_restart_btn', () => {
    console.log("restart gsm modem");
    // //requestUpdate(wsEvents.gsm.GsmATCmd, {cmd: "+CPMS?"});
    // requestUpdate(wsEvents.gsm.GsmATCmd, {cmd: "+CMEE=2"});
    
    // requestUpdate(wsEvents.gsm.GsmATCmd, {cmd: "+CPMS?"});
    // requestUpdate(wsEvents.gsm.GsmATCmd, {cmd: "+CSDH?"});
    
    // //requestUpdate(wsEvents.gsm.GsmATCmd, {cmd: "+CPMS=\"ME_P\""});
    
    // //requestUpdate(wsEvents.gsm.GsmATCmd, {cmd: "+CNMI?"});
    // //requestUpdate(wsEvents.gsm.GsmATCmd, {cmd: "+CNMA=0"});
    // requestUpdate(wsEvents.gsm.GsmATCmd, {cmd: "+CNMI=2,2,0,1,0"});
    // // //requestUpdate(wsEvents.gsm.GsmATCmd, {cmd: "+CMGD=2,0"});
    // requestUpdate(wsEvents.gsm.GsmATCmd, {cmd: "+CMGL=4"});
    // //requestUpdate(wsEvents.gsm.GsmATCmd, {cmd: "+CMGL=\"ALL\""});
    

    // sendAT(GF("+CMGF=1"));
    // sendAT(String(GF("+CSCS=\"")) + format + GF("\""));
    // //waitResponse();
    // sendAT(String(GF("+CUSD=1,\"")) +  code + GF("\""));

     //requestUpdate(wsEvents.gsm.GsmATCmd, {cmd: "+CMGF=1"});
     //requestUpdate(wsEvents.gsm.GsmATCmd, {cmd: "+CSCS=\"GSM\""});
    //requestUpdate(wsEvents.gsm.GsmATCmd, {cmd: "+CUSD=1,\"*100#\""});
    requestUpdate(wsEvents.gsm.UpdateBalance);
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

/**
 * Обновление баланса
 */
onclick('#gsm_balance_btn', () => {
    requestUpdate(wsEvents.gsm.UpdateBalance);
});

window.onblur = function(event) {
    setNetworkUpdateInfo(false);
};

window.onfocus = function(event) {
    setNetworkUpdateInfo(true);
};

// включаем автообновление состояния сети
const setNetworkUpdateInfo = (toUpdate) => {
    requestUpdate(wsEvents.gsm.GsmEnableUpdateNetworkInfo, {value: toUpdate});
};

const updateNetworkInfo = (netInfo) => {
    networkGsmInfo =  {...networkGsmInfo, ...netInfo};
    renderNetworkInfo();
}
const setNetworkInfo = (netInfo) => {
    networkGsmInfo[netInfo.key] = netInfo.value;
    renderNetworkInfo();
}

const renderNetworkInfo = () => {
    changeValue("#gsm_status", parseRegNetworkParam());
}

const parseRegNetworkParam = () => {
    const reg = networkGsmInfo.reg;
    if(reg == 0) return "Не найдена gsm сеть";
    if(reg == 1){
        if(!networkGsmInfo.signal)
            networkGsmInfo.signal = 0;
        if(!networkGsmInfo.operator)
            networkGsmInfo.operator = "";
        return "Сеть: " + networkGsmInfo.operator + " / " + networkGsmInfo.signal;
    } 
     
    if(reg == 2)
        return "Поиск сети " + networkGsmInfo.operator + " / " + networkGsmInfo.signal;
    if(reg == 3)
        return "Отказ регистрации " + networkGsmInfo.operator + " / " + networkGsmInfo.signal;
    if(reg == 4)
        return "Обновление статуса";
    return "Нет связи с модемом";
}

regOnMessage(wsEvents.gsm.UpdateStatus, (statusGsm) => {
    console.log('gsm_status changed to ' + statusGsm); 
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
    openModal(msg);
});

regOnOpen(() => {
    //requestUpdate(wsEvents.gsm.UpdateStatus);
    requestUpdate(wsEvents.gsm.UpdateBattPercent);
    requestUpdate(wsEvents.gsm.NetworkInfo);
    requestUpdate(wsEvents.gsm.GsmEnableUpdateNetworkInfo, {value: true});
});
