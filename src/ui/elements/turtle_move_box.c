#include "turtle_move_box.h"
#include "../common.h"

TurtleMoveBox mk_turtle_move_box(){
    Turtlebox turt = mk_turtlebox();
    if (turt.error) return TURTLE_MOVE_BOX_ERR;
    Module * turtle_out = mk_module(MODULE_OUTPUT, MODULE_DATA_TYPE_turtle);
    if (turtle_out->type == MODULE_NONE) {
        free_turtlebox(turt);
        return TURTLE_MOVE_BOX_ERR;
    }
    TurtleMoveBox tb = (TurtleMoveBox){
        .turtlebox = turt 
       , .movecontainer = mk_move_container(NULL, NULL)
       , .turtle_out = turtle_out
       };
    tb.turtle_out->output.turtle = tb.turtlebox.turtle;
    tb.turtle_out->output.valid = true;
    tb.movecontainer.size = (Vector2){.x = 160, .y=510};
    return tb;
}

void free_turtle_move_box(TurtleMoveBox tb){
    free_turtlebox(tb.turtlebox);
}

UpdateReturnValue update_turtle_move_box(TurtleMoveBox * tb){
     UpdateReturnValue res;
    if ((res = update_module(tb->turtle_out)).interacted) return res;
    if ((res = update_turtlebox(&tb->turtlebox)).interacted) {
        tb->turtle_out->output.turtle = tb->turtlebox.turtle;
        tb->turtle_out->output.valid = true;
        return res;
    }
    if ((res = update_move_container(&tb->movecontainer, false, NULL)).interacted) return res;
    return UPDATE_NONE;
}

void layout_turtle_move_box(TurtleMoveBox tb, Font * fonts){
/*    CLAY(move_cont_clay_id(tb.movecontainer), move_cont_clay_decl(tb.movecontainer, 8, false)){
            layout_turtlebox(tb.turtlebox, fonts, "Turtle");
            CLAY_AUTO_ID({.border = {.color = COL_DARK, .width = {0,0,0,0,1}}}){
                layout_module(*tb.turtle_out);
            };
        };
        */

    LAYOUT_MOVE_CONTAINER(tb.movecontainer, 

            layout_turtlebox(tb.turtlebox, fonts, "Turtle");
            CLAY_AUTO_ID({.border = 
                {.color = COL_DARK, .width = {0,0,0,0,1}}}){
                layout_module(*tb.turtle_out);
            };

           , 

           CLAY_AUTO_ID({.border = 
               {.color = COL_DARK, .width = {0,0,0,0,1}}}){
                layout_module(*tb.turtle_out);
            };

    , 8, false);
}
