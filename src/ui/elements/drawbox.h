#ifndef DRAWBOX_H
#define DRAWBOX_H

#include "../module.h"
#include "../move_container.h"

typedef struct {
    bool error;
    int update_handle;
    Move_container cont;
    Module * lstring_in;
    Module * turtle_in;
} DrawBox;

DrawBox mk_drawbox();
void free_drawbox(DrawBox db);
bool update_drawbox(DrawBox * db);
void layout_drawbox(DrawBox db);

#define ERR_DRAWBOX (DrawBox) {true, -1, {0}, ERR_MODULE, ERR_MODULE}

#endif 
