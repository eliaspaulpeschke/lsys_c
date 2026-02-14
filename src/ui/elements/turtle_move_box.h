#ifndef TURTLE_MOVE_BOX_H
#define TURTLE_MOVE_BOX_H

#include "../turtlebox.h"
#include "../move_container.h"
#include "../module.h"
#include "raylib.h"

typedef struct {
    bool error;
    Turtlebox turtlebox;
    Move_container movecontainer;
    Module turtle_out;
} TurtleMoveBox;

TurtleMoveBox mk_turtle_move_box();
void free_turtle_move_box(TurtleMoveBox tb);
bool update_turtle_move_box(TurtleMoveBox * tb);
void layout_turtle_move_box(TurtleMoveBox tb, Font * fonts);
#define TURTLE_MOVE_BOX_ERR (TurtleMoveBox){.error=true}

#endif
