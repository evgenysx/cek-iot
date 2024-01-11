
const gsmEvents = {
    UpdateStatus: 'GsmUpdateStatus',
    UpdateBattPercent: 'GsmUpdateBattPercent',
    UpdateBalance: 'GsmUpdateBalance',
    UpdateSignalQuality: 'GsmUpdateSignalQuality',
    SendSMS: 'GsmSendSMS',
    CallNumber: 'GsmCallNumber'
};

const deviceEvents = {
    RestartDevice: 'RestartDevice',
};

const wsEvents = {
    gsm: gsmEvents,
    device: deviceEvents,
    
};

export {wsEvents};