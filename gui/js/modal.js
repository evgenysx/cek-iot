import {changeValue} from './dom'

/**
 * Открывает диалоговое окно
 * @param {*} text 
 */
const openModal = (text) => {
    changeValue('#mdl_text_val', text);
    window.location = '#css-modal-target';
}

export {openModal}