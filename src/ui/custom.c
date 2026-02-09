#include "custom.h"
#include <string.h>

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
