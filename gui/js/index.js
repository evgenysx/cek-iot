// стили
import "../css/style.css";
import "../css/modal.css";
import "../css/switch.css";

//###
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
