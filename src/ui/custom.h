#ifndef CUSTOM_H
#define CUSTOM_H
#include "elements/turtle_move_box.h"
#include "elements/lrules_box.h"

#define ERR_CUSTOM_ELEM (CustomElementData){true, 0, 0}

typedef enum {
      CUSTOM_ELEM_T_rulesbox
    , CUSTOM_ELEM_T_turtle_box
} CustomElementType;

typedef struct {
    bool error;
    CustomElementType type;
    union {
        LRulesBox rulesbox;
        TurtleMoveBox turtlebox;
    };
} CustomElementData;

CustomElementData mk_rulesbox_elem();
CustomElementData mk_turtlebox_elem();

void free_custom_elem(CustomElementData ced);
#endif
