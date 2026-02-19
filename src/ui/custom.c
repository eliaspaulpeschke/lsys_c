#include "custom.h"
#include "elements/apply_box.h"
#include "elements/lrules_box.h"

CustomElementData mk_rulesbox_elem(){
    CustomElementData ced = (CustomElementData) {
                   .type = CUSTOM_ELEM_T_rulesbox,
                   .rulesbox = mk_lrules_box()
                   };
    if (ced.rulesbox.error) {
        return ERR_CUSTOM_ELEM;
    }
    return ced;
}

CustomElementData mk_applybox_elem(){
    CustomElementData ced = (CustomElementData) {
                   .type = CUSTOM_ELEM_T_apply_box,
                   .applybox = mk_applybox()
                   };
    if (ced.rulesbox.error) {
        return ERR_CUSTOM_ELEM;
    }
    return ced;
}

CustomElementData mk_turtlebox_elem(){
    CustomElementData ced = (CustomElementData) {
                     .error = false
                   , .type = CUSTOM_ELEM_T_turtle_box
                   , .turtlebox = mk_turtle_move_box()
                   };
    if (ced.turtlebox.error) return ERR_CUSTOM_ELEM;
    return ced;
}

void free_custom_elem(CustomElementData ced){
    switch (ced.type) {
        case CUSTOM_ELEM_T_rulesbox:
            free_lrules_box(ced.rulesbox);
            return;
        case CUSTOM_ELEM_T_turtle_box:
            free_turtle_move_box(ced.turtlebox);
            return;
        case CUSTOM_ELEM_T_apply_box:
            free_applybox(ced.applybox);
            return;
        default:
            return;
    }
}
