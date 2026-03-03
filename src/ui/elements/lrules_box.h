#ifndef LRULES_BOX_H
#define LRULES_BOX_H

#include "../textbox.h"
#include "../move_container.h"
#include "../module.h"
#include "../button.h"
#include "raylib.h"

typedef struct {
    bool error;
    Textbox textbox;
    Move_container movecontainer;
    Module * lsys_out;
    ButtonElem button_parse;
} LRulesBox;

LRulesBox mk_lrules_box();
void free_lrules_box(LRulesBox tb);
UpdateReturnValue update_lrules_box(LRulesBox * tb);
void layout_lrules_box(LRulesBox tb, Font * fonts);
#define LRULES_BOX_ERR (LRulesBox){.error=true}

#endif
