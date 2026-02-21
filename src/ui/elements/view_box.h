#ifndef VIEWBOX_H
#define VIEWBOX_H

#include "../textbox.h"
#include "../module.h"
#include "../move_container.h"

typedef struct {
    Textbox textbox;
    Module * lstring_in;
    Module * lstring_out;
    Move_container cont;
} ViewBox;

ViewBox mk_viewbox();
void free_viewbox(ViewBox vb);
bool update_viewbox(ViewBox * vb);
void layout_viewbox(ViewBox vb);
#endif
