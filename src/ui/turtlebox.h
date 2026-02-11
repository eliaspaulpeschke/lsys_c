#ifndef TURTLEBOX_H
#define TURTLEBOX_H

#include "../turtle/turtle.h"
#include "vectorbox.h"
#include "inputbox.h"

#define ERR_TURTLEBOX (Turtlebox){true, 0, 0, 0, 0, 0, 0}

typedef struct {
    bool error;
    Turtle turtle;
    Vector2_box pos;
    Vector2_box heading;
    Inputbox length;
    Inputbox turn_rads;
    Vector4_box color;
} Turtlebox;


Turtlebox mk_turtlebox();
bool update_turtlebox(Turtlebox * tb);
void layout_turtlebox(Turtlebox tb, Font * fonts, char * label);
void free_turtlebox(Turtlebox tb);

#endif
