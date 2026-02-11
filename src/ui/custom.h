#ifndef CUSTOM_H
#define CUSTOM_H
#include "textbox.h"
#include "turtlebox.h"

#define ERR_CUSTOM_ELEM (CustomElementData){true, 0, 0}

typedef enum {
      CUSTOM_ELEM_T_textbox
    , CUSTOM_ELEM_T_turtle_box
} CustomElementType;

typedef struct {
    bool error;
    CustomElementType type;
    union {
        Textbox textbox;
        Turtlebox turtlebox;
    };
} CustomElementData;

CustomElementData mk_textbox_elem(unsigned int max_len);
CustomElementData mk_turtlebox_elem();

void free_custom_elem(CustomElementData ced);
#endif
