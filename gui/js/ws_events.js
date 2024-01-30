
const gsmEvents = {
    UpdateStatus: 'GsmUpdateStatus',
    UpdateBattPercent: 'GsmUpdateBattPercent',
    UpdateBalance: 'GsmUpdateBalance',
    UpdateSignalQuality: 'GsmUpdateSignalQuality',
    SendSMS: 'GsmSendSMS',
    CallNumber: 'GsmCallNumber',
    NetworkInfo: 'GsmNetworkInfo',
    GsmGetLocation: 'GsmGetLocation',
    RestartModem: 'GsmRestartModem',
    GsmATCmd: 'GsmATCmd',
    GsmEnableUpdateNetworkInfo: 'GsmEnableUpdateNetworkInfo',
};

const deviceEvents = {
    EnableLog: 'EnableLog',
    DisableLog: 'DisableLog',
    PrintLog: 'PrintLog',
    RestartDevice: 'RestartDevice',
};

const wsEvents = {
    gsm: gsmEvents,
    device: deviceEvents,
    
};

export {wsEvents};