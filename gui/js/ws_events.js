
const gsmEvents = {
    UpdateStatus: 'GsmUpdateStatus',
    UpdateBattPercent: 'GsmUpdateBattPercent',
    UpdateBalance: 'GsmUpdateBalance',
    UpdateSignalQuality: 'GsmUpdateSignalQuality',
    SendSMS: 'GsmSendSMS',
    CallNumber: 'GsmCallNumber',
    NetworkInfo: 'GsmNetworkInfo',
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