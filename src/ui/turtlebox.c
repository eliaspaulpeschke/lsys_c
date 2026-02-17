#include "turtlebox.h"
#include "../clay/clay.h"
#include <stdbool.h>
#include <stdlib.h>
#include "common.h"
#include "vectorbox.h"

Turtlebox mk_turtlebox(){
    Turtle turtle = mk_base_turtle();
    Vector2_box pos = mk_vector2box();
    if (pos.error == true) return ERR_TURTLEBOX;
    Vector2_box heading = mk_vector2box();
    if (heading.error == true) {
        free_vector2box(pos);
        return ERR_TURTLEBOX;
    }
    Vector1_box length = mk_vector1box();
    if (length.error == true) {
        free_vector2box(heading);
        free_vector2box(pos);
        return ERR_TURTLEBOX;
    }
    Vector1_box turn_rads = mk_vector1box();
    if (turn_rads.error == true) {
        free_vector1box(length);
        free_vector2box(heading);
        free_vector2box(pos);
        return ERR_TURTLEBOX;
    }
    Vector4_box color = mk_vector4box();
    if (color.error == true) {
        free_vector1box(length);
        free_vector1box(turn_rads);
        free_vector2box(heading);
        free_vector2box(pos);
        return ERR_TURTLEBOX;
    }
    return (Turtlebox){
        .error = false
      , .turtle = turtle
      , .pos = pos
      , .heading = heading
      , .length = length
      , .turn_rads = turn_rads
      , .color = color
    };
}

void free_turtlebox(Turtlebox tb){
    free_vector2box(tb.pos);
    free_vector2box(tb.heading);
    free_vector1box(tb.length);
    free_vector1box(tb.turn_rads);
    free_vector4box(tb.color);
}

bool update_turtlebox(Turtlebox *tb){
    if (update_vector2box(&tb->pos)){
        tb->turtle.pos = tb->pos.value;
        return true;
    }
    if (update_vector2box(&tb->heading)){
        tb->turtle.heading = tb->heading.value;
        return true;
    }
    if (update_vector1box(&tb->length)){
        tb->turtle.length = tb->length.value;
        return true;
    }
    if (update_vector1box(&tb->turn_rads)){
        tb->turtle.rads = tb->turn_rads.value; 
        return true;
    }
    if (update_vector4box(&tb->color)){
        Vector4 c = tb->color.value;
        tb->turtle.color = (Color){
              .r = c.x
            , .g = c.y
            , .b = c.z
            , .a = c.w
        };
        return true;
    }
    return false;
}

void layout_turtlebox(Turtlebox tb, Font * fonts, char * label){
    CLAY_AUTO_ID({
        .layout = { .sizing = { CLAY_SIZING_GROW(0)
                              , CLAY_SIZING_GROW(0)}
                  , .childGap = 8
                  , .layoutDirection = CLAY_TOP_TO_BOTTOM
                  , .childAlignment = { .x = CLAY_ALIGN_X_LEFT
                                      , .y = CLAY_ALIGN_Y_TOP }
                  } 
      , .backgroundColor = COL_LIGHT
      , .border = {.width = {0,0,0,0,1}, .color = COL_DARK}
    }){
        add_label(label);
        layout_vector2box(tb.pos, fonts, "Position");
        layout_vector2box(tb.heading, fonts, "Heading");
        layout_vector1box(tb.length, fonts, "Step Length");
        layout_vector1box(tb.turn_rads, fonts, "Turn Radians");
        layout_vector4box(tb.color, fonts,true, "Color");
    };
}

