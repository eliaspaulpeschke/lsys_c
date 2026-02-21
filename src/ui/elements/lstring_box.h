#ifndef LSTRING_BOX_H
#define LSTRING_BOX_H

#include "../textbox.h"
#include "../move_container.h"
#include "../module.h"
#include "../button.h"
#include "raylib.h"

typedef struct {
    bool error;
    Textbox textbox;
    Move_container movecontainer;
    Module * lstring_out;
    ButtonElem button_parse;
} LStringBox;

LStringBox mk_lstring_box();
void free_lstring_box(LStringBox lb);
bool update_lstring_box(LStringBox * lb);
void layout_lstring_box(LStringBox lb, Font * fonts);
#define LSTRING_BOX_ERR (LStringBox){.error=true}

#endif
