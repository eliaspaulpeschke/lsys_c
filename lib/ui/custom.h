#ifndef CUSTOM_H
#define CUSTOM_H
#include "../clay.h"
#include "./textbox.h"
#include "common.h"
#include "inputbox.h"
#include "raylib.h"
#include <stdlib.h>
#include <string.h>

typedef enum {
    CUSTOM_ELEM_T_textbox
} CustomElementType;

typedef struct {
    CustomElementType type;
    union {
        textbox textbox;
    };
} CustomElementData;

CustomElementData * mk_textbox_elem(uint max_len){
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
#endif
