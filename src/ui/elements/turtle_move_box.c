#include "turtle_move_box.h"
#include "../common.h"

TurtleMoveBox mk_turtle_move_box(){
    Turtlebox turt = mk_turtlebox();
    if (turt.error) return TURTLE_MOVE_BOX_ERR;
    TurtleMoveBox tb = (TurtleMoveBox){
        .turtlebox = turt 
       , .movecontainer = mk_move_container()
       , .turtle_out = mk_module(MODULE_OUTPUT, MODULE_DATA_TYPE_turtle)
    };
    tb.movecontainer.size = (Vector2){.x = 160, .y=510};
    return tb;
}

void free_turtle_move_box(TurtleMoveBox tb){
    free_turtlebox(tb.turtlebox);
}

bool update_turtle_move_box(TurtleMoveBox * tb){
    if (update_move_container(&tb->movecontainer, false)) return true;
    if (update_turtlebox(&tb->turtlebox)) {
        tb->turtle_out.output.turtle = tb->turtlebox.turtle;
        tb->turtle_out.output.valid = true;
        return true;
    }
    return false;
}

void layout_turtle_move_box(TurtleMoveBox tb, Font * fonts){
    CLAY(move_cont_clay_id(tb.movecontainer), move_cont_clay_decl(tb.movecontainer, false)){
            layout_turtlebox(tb.turtlebox, fonts, "Turtle");
            CLAY_AUTO_ID({.border = {.color = COL_DARK, .width = {0,0,0,0,1}}}){
            layout_module(&tb.turtle_out);
            }
        };
}
