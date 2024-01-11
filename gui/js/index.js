import './gsm'
import './sensors'
import { onclick } from "./dom";

import  {requestUpdate} from './ws'
import { wsEvents } from "./ws_events";

onclick('#esp_restart_btn', () => {
    requestUpdate(wsEvents.device.RestartDevice);
});