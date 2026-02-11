#include "turtlebox.h"
#include "../clay/clay.h"
#include <stdbool.h>
#include <stdlib.h>
#include "common.h"

Turtlebox mk_turtlebox(){
    Turtle turtle = mk_base_turtle();
    Vector2_box pos = mk_vector2box();
    if (pos.error == true) return ERR_TURTLEBOX;
    Vector2_box heading = mk_vector2box();
    if (heading.error == true) {
        free_vector2box(pos);
        return ERR_TURTLEBOX;
    }
    Inputbox length = mk_inputbox(32);
    if (length.error == true) {
        free_vector2box(heading);
        free_vector2box(pos);
        return ERR_TURTLEBOX;
    }
    Inputbox turn_rads = mk_inputbox(32);
    if (turn_rads.error == true) {
        free_inputbox(length);
        free_vector2box(heading);
        free_vector2box(pos);
        return ERR_TURTLEBOX;
    }
    Vector4_box color = mk_vector4box();
    if (color.error == true) {
        free_inputbox(length);
        free_inputbox(turn_rads);
        free_vector2box(heading);
        free_vector2box(pos);
        return ERR_TURTLEBOX;
    }
    set_inputbox_text(length, "0.0");
    set_inputbox_text(turn_rads, "0.0");
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
    free_inputbox(tb.length);
    free_inputbox(tb.turn_rads);
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
    if (update_inputbox(&tb->length)){
        if (!tb->length.changed) return true;
        tb->turtle.length = strtof(tb->length.text, NULL);
        return true;
    }
    if (update_inputbox(&tb->turn_rads)){
        if (!tb->turn_rads.changed) return true;
        tb->turtle.rads= strtof(tb->turn_rads.text, NULL);
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
                  , .padding = 8
                  , .childAlignment = { .x = CLAY_ALIGN_X_LEFT
                                      , .y = CLAY_ALIGN_Y_TOP }
                  } 
      , .backgroundColor = COL_LIGHT
      , .border = {.width = CLAY_BORDER_ALL(1), .color = COL_DARK}
    }){
        add_label(label);
        layout_vector2box(tb.pos, fonts, "Position");
        layout_vector2box(tb.heading, fonts, "Heading");
        layout_inputbox(tb.length, fonts, false, "Length");
        layout_inputbox(tb.turn_rads, fonts, false, "Turn Radians");
        layout_vector4box(tb.color, fonts, "Color");
    };
}

