import './gsm'
import './sensors'
import { onclick } from "./dom";

import  {requestUpdate, wsEvents} from './ws'

onclick('#esp_restart_btn', () => {
    requestUpdate(wsEvents.device.RestartDevice);
});