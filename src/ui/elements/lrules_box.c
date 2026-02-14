#include "lrules_box.h"
#include "../common.h"

LRulesBox mk_lrules_box(){
    Textbox tb = mk_textbox(2048);
    if (tb.error) return LRULES_BOX_ERR;
    LRulesBox lb = (LRulesBox){
        .textbox = tb 
       , .movecontainer = mk_move_container()
       , .lsys_out = mk_module(MODULE_OUTPUT, MODULE_DATA_TYPE_ruleset)
    };
    lb.movecontainer.size = (Vector2){.x = 160, .y=510};
    return lb;
}

void free_lrules_box(LRulesBox lb){
    free_textbox(lb.textbox);
}

bool update_lrules_box(LRulesBox * lb){
    if (update_move_container(&lb->movecontainer, false)) return true;
    if (update_textbox(&lb->textbox, false)) return true;
    return false;
}

void layout_lrules_box(LRulesBox lb, Font * fonts){
    CLAY(move_cont_clay_id(lb.movecontainer), move_cont_clay_decl(lb.movecontainer, false)){
            CLAY_AUTO_ID({.border = {.color = COL_DARK, .width = CLAY_BORDER_OUTSIDE(1)}}){
            layout_module(&lb.lsys_out);
            }
        };
}
