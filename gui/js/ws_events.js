
const gsmEvents = {
    UpdateStatus: 'GsmUpdateStatus',
    UpdateBattPercent: 'GsmUpdateBattPercent',
    UpdateBalance: 'GsmUpdateBalance',
    UpdateSignalQuality: 'GsmUpdateSignalQuality',
    SendSMS: 'GsmSendSMS',
    CallNumber: 'GsmCallNumber'
};

const wsEvents = {
    gsm : gsmEvents
    
};

export {wsEvents};