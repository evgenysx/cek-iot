 // as jQuery
 let $ = function(s){
  //or querySelector for one element depends on need
  return document.querySelector(s);
};

 /**
   * Регистрирует обработчик события click
   */
 const onclick = (id, handlerEvent) => {
   if(typeof handlerEvent !== "function"){
    console.error("no handler", handlerEvent)
   }
    const el = $(id);
  
    if (el) {
      el.addEventListener('click', (e) => {
        el.classList.toggle("switch-on")
        //
        handlerEvent(el);
      });
    }else
      console.error("$ no byId ", id)
  }


  export {onclick}