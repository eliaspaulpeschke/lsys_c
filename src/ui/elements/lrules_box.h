#ifndef LRULES_BOX_H
#define LRULES_BOX_H

#include "../textbox.h"
#include "../move_container.h"
#include "../module.h"
#include "raylib.h"

typedef struct {
    bool error;
    Textbox textbox;
    Move_container movecontainer;
    Module lsys_out;
} LRulesBox;

LRulesBox mk_lrules_box();
void free_lrules_box(LRulesBox tb);
bool update_lrules_box(LRulesBox * tb);
void layout_lrules_box(LRulesBox tb, Font * fonts);
#define LRULES_BOX_ERR (LRulesBox){.error=true}

#endif
