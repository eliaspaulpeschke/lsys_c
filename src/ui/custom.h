#ifndef CUSTOM_H
#define CUSTOM_H
#include "elements/turtle_move_box.h"
#include "elements/lrules_box.h"
#include "elements/apply_box.h"
#include "elements/lstring_box.h"

#define ERR_CUSTOM_ELEM (CustomElementData){true, 0, 0}

typedef enum {
      CUSTOM_ELEM_T_rulesbox
    , CUSTOM_ELEM_T_turtle_box
    , CUSTOM_ELEM_T_lstring_box
    , CUSTOM_ELEM_T_apply_box
} CustomElementType;

typedef struct {
    bool error;
    CustomElementType type;
    union {
        LRulesBox rulesbox;
        TurtleMoveBox turtlebox;
        ApplyBox applybox;
        LStringBox lstringbox;
    };
} CustomElementData;

CustomElementData mk_rulesbox_elem();
CustomElementData mk_turtlebox_elem();
CustomElementData mk_applybox_elem();
CustomElementData mk_lstringbox_elem();


void free_custom_elem(CustomElementData ced);
#endif
