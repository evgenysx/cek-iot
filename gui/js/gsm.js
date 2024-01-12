/**
 * Вкладка GSM
 */

import { onclick, changeValue } from "./dom";
import {notifyToggleCheckbox, regOnMessage, regOnOpen, requestUpdate, wsEvents} from './ws'


onclick('#gsm_gprs_enabled', notifyToggleCheckbox);

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


regOnOpen(() => {
    //requestUpdate(wsEvents.gsm.UpdateBattPercent);
    requestUpdate(wsEvents.gsm.UpdateStatus);
    requestUpdate(wsEvents.gsm.UpdateSignalQuality);
    requestUpdate(wsEvents.gsm.NetworkInfo);
});
