import './ws_events'

export {wsEvents} from './ws_events';

/**
 * Список зареганых обработчиков
 */
let mapCallbackEvents = new Map();
let setCallbackOnOpen = new Set();

var wsClient;

const connectWsApi = () => {
  wsClient = new WebSocket("ws://" + /*window.location.host*/ "192.168.3.89" + "/ws");

  wsClient.onopen = function() {
    console.log("Ws-cоединение установлено.");
    for (let onOpenCallback of setCallbackOnOpen) {
      onOpenCallback(); 
    }
  };

  wsClient.onclose = function(event) {
    setTimeout(() => {
       if (!event.wasClean){
        console.log("try ws-reconnect");
        connectWsApi();
       }
        
    }, 10*1000);

  };

  wsClient.onmessage = function(event) {
    console.log("new message : " + event.data)
    const msg = JSON.parse(event.data).data;
    for (let listener of mapCallbackEvents) {
      listener[1](msg); 
    }
  };

  wsClient.onerror = function(error) {
    
  };

};

const notifyToggleCheckbox = (el) => {
  let event = {id: el.id, data:el.checked}
  console.log(event)
  wsClient.send(JSON.stringify(event));

}

const requestUpdate = (msg) => {
  let event = {type: msg}
  wsClient.send(JSON.stringify(event));

}

/**
 * Регистрация callback на сообщение от сервера
 * @param {*} type 
 * @param {*} callback 
 */
const regOnMessage = (type, callback) => {
  mapCallbackEvents.set(type, callback); 
}

/**
 * Подключаемся к бекенду
 */
connectWsApi();

const regOnOpen = (callback) => {
  setCallbackOnOpen.add(callback);
}
export { notifyToggleCheckbox, regOnMessage, regOnOpen, requestUpdate};