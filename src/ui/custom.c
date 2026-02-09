#include "custom.h"
#include "textbox.h"
#include <string.h>
#include <stdlib.h>

CustomElementData * mk_textbox_elem(unsigned int max_len){
    CustomElementData * ced = 
        malloc(sizeof(CustomElementData));
    if (!ced) return NULL;
    *ced = (CustomElementData) {
                   .type = CUSTOM_ELEM_T_textbox,
                   .textbox = mk_textbox(max_len)
                   };
    if (!ced->textbox.text) {
        free(ced);
        return NULL;
    }
    return ced;
}

void free_custom_elem(CustomElementData ced){
    switch (ced.type) {
        case CUSTOM_ELEM_T_textbox:
            free_textbox(ced.textbox);
            return;
        default:
            return;
    }
}
