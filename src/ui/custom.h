#ifndef CUSTOM_H
#define CUSTOM_H
#include "textbox.h"

typedef enum {
    CUSTOM_ELEM_T_textbox
} CustomElementType;

typedef struct {
    CustomElementType type;
    union {
        textbox textbox;
    };
} CustomElementData;

CustomElementData * mk_textbox_elem(unsigned int max_len);
#endif
