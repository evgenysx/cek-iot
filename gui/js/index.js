import './gsm'
import './sensors'
import { onclick, addRow } from "./dom";

import  {regOnMessage, requestUpdate, wsEvents} from './ws'

onclick('#esp_restart_btn', () => {
    requestUpdate(wsEvents.device.RestartDevice);
});


regOnMessage(wsEvents.device.PrintLog, (msg) => {
    addRow("#esp32Log", msg);
});
