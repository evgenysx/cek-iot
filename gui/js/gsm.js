/**
 * Вкладка GSM
 */

import { onclick, changeValue } from "./dom";
import {notifyToggleCheckbox, regOnMessage, regOnOpen, requestUpdate, wsEvents} from './ws'


onclick('#gsm_gprs_enabled', notifyToggleCheckbox);
onclick('#gsm_restart_btn', () => {
    console.log("restart gsm modem");
    requestUpdate(wsEvents.gsm.RestartModem);
});

onclick('#gsm_sendsms_btn', () => {
    console.log("send SMS");
    requestUpdate(wsEvents.gsm.GsmATCmd, {p1:"test data"});
});


regOnMessage(wsEvents.gsm.UpdateStatus, (msg) => {
    console.log('gsm_status changed ' + msg);
    changeValue("#gsm_status", msg);
});

regOnMessage(wsEvents.gsm.UpdateBattPercent, (msg) => {
    console.log('GsmUpdateBattPercent ' + msg);
    changeValue("#gsm_battery", msg);
});

regOnMessage(wsEvents.gsm.NetworkInfo, (msg) => {
    console.log('GsmNetworkInfo ' + msg);
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
    //requestUpdate(wsEvents.gsm.UpdateBattPercent);
    requestUpdate(wsEvents.gsm.UpdateStatus);
    requestUpdate(wsEvents.gsm.UpdateSignalQuality);
    requestUpdate(wsEvents.gsm.NetworkInfo);
    //requestUpdate(wsEvents.gsm.GsmGetLocation);
    //requestUpdate(wsEvents.gsm.UpdateBalance);
    //requestUpdate(wsEvents.gsm.SendSMS);
});
