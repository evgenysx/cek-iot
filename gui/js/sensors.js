import { onclick } from "./dom";
import { notifyToggleCheckbox } from './ws'

onclick('#relay_enabled', notifyToggleCheckbox);