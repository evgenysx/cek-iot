/**
 * Вкладка GSM
 */

import { onclick } from "./dom";
import {notifyToggleCheckbox} from './ws'

onclick('#gsm_gprs_enabled', notifyToggleCheckbox);