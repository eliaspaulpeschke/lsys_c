#include "custom.h"
#include "elements/turtle_move_box.h"
#include "textbox.h"
#include <string.h>
#include <stdlib.h>

CustomElementData mk_textbox_elem(unsigned int max_len){
    CustomElementData ced = (CustomElementData) {
                   .type = CUSTOM_ELEM_T_textbox,
                   .textbox = mk_textbox(max_len)
                   };
    if (!ced.textbox.text) {
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
        case CUSTOM_ELEM_T_textbox:
            free_textbox(ced.textbox);
            return;
        case CUSTOM_ELEM_T_turtle_box:
            free_turtle_move_box(ced.turtlebox);
        default:
            return;
    }
}
