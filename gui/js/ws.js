var socket = new WebSocket("ws://" + window.location.host + "/ws");
  
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
    
  };

  socket.onerror = function(error) {
    
  };
  };

  function notifyToggleCheckbox(element) {
    
    let event = {id: element.id, data:element.checked}
    console.log(event)

    socket.send(JSON.stringify(event));

  }

  connectWsApi();

  export { notifyToggleCheckbox};