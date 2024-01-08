import './ws_events'

var socket = new WebSocket("ws://" + window.location.host + "/ws");
/**
 * Список зареганых обработчиков
 */
let mapCallbackEvents = new Map();

const connectWsApi = () => {
  socket.onopen = function() {
    console.log("Ws-cоединение установлено.");
  };

  socket.onclose = function(event) {
    if (event.wasClean) {

    } else {
      
    }

  };

  socket.onmessage = function(event) {
    console.log("new message : " + event.data)
    const msg = JSON.parse(event.data).data;
    for (let listener of mapCallbackEvents) {
      console.log(listener)
      listener[1](msg); 
    }
  };

  socket.onerror = function(error) {
    
  };
};

const notifyToggleCheckbox = (el) => {
  let event = {id: el.id, data:el.checked}
  console.log(event)
  socket.send(JSON.stringify(event));

}

const requestUpdate = (msg) => {
  let event = {e: msg}
  socket.send(JSON.stringify(event));

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

export { notifyToggleCheckbox, regOnMessage, requestUpdate};