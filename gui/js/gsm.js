/**
 * Вкладка GSM
 */

import { onclick, changeValue } from "./dom";
import {notifyToggleCheckbox, regOnMessage, regOnOpen, requestUpdate} from './ws'
import { wsEvents } from "./ws_events";

onclick('#gsm_gprs_enabled', notifyToggleCheckbox);

regOnMessage(wsEvents.gsm.UpdateStatus, (msg) => {
    console.log('gsm_status changed ' + msg);
    changeValue("#gsm_status", msg);
});

regOnMessage(wsEvents.gsm.UpdateBattPercent, (msg) => {
    console.log('GsmUpdateBattPercent ' + msg);
    changeValue("#gsm_battery", msg);
});

regOnOpen(() => {
    //requestUpdate(wsEvents.gsm.UpdateBattPercent);
    requestUpdate(wsEvents.gsm.UpdateStatus);
    requestUpdate(wsEvents.gsm.UpdateSignalQuality);
});
