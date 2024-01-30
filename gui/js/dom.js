 // as jQuery
 let $ = function(s){
  const el = document.querySelector(s);
  if(!el)
    console.error("$ no byId ", id)
  //or querySelector for one element depends on need
  return el;
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
  }
}

/**
 * Изменяет внутренний текст между тегами
 * @param {*} id 
 * @param {*} value 
 */
const changeValue = (id, value) => {
  const el = $(id);
  el.innerHTML = value;
}

const addRow = (id, value) => {
  const el = $(id);
  el.innerHTML += '<p>' + value + '</p>';
}

const getValue = (id) => {
  const el = $(id);
  return el.value;
}

const getAttr = (id, attr) => {
  const el = $(id);
  return el.getAttribute(attr);
}
export {onclick, changeValue, getValue, getAttr, addRow}