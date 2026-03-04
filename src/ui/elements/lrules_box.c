#include "lrules_box.h"
#include "../common.h"
#include "../../lsystem/parser/lsystem_parser.h"
#include "raylib.h"

static void on_click_parse_btn(void * user_data);

LRulesBox mk_lrules_box(){
    Textbox tb = mk_textbox(2048);
    if (tb.error) return LRULES_BOX_ERR;
    Module * lsys_out = mk_module(MODULE_OUTPUT, MODULE_DATA_TYPE_ruleset);
    if (lsys_out->type == MODULE_NONE) {
        free_textbox(tb);
        return LRULES_BOX_ERR;
    }
    LRulesBox lb = (LRulesBox){
        .textbox = tb 
       , .movecontainer = mk_move_container(NULL, NULL)
       , .lsys_out = lsys_out
       , .button_parse = mk_button("parse", on_click_parse_btn)
    };
    lb.button_parse.size.x = 56;
    lb.movecontainer.size = (Vector2){.x = 256, .y=384};
    return lb;
}

void lrules_box_parse(LRulesBox * lb){
    if (lb->textbox.text == NULL || strlen(lb->textbox.text) == 0) {
        lb->lsys_out->output.valid = false;
        return;
    }
    LRuleset rules = parse_string_to_ruleset(lb->textbox.text, 32);
    if (rules.rules == NULL) {
        lb->lsys_out->output.valid = false;
        return;
    }
    lb->lsys_out->output.ruleset = rules;
    lb->lsys_out->output.valid = true;

}

static void on_click_parse_btn(void * user_data){
    LRulesBox * lb = (LRulesBox *)user_data;
    lrules_box_parse(lb);
}

void free_lrules_box(LRulesBox lb){
    free_textbox(lb.textbox);
}

UpdateReturnValue update_lrules_box(LRulesBox * lb){
    UpdateReturnValue res;
    if ((res = update_module(lb->lsys_out)).interacted) return res;
    if ((res = update_textbox(&lb->textbox, false)).interacted) return res;
    if ((res = update_button(&lb->button_parse, lb)).interacted) return res;
    if ((res = update_move_container(&lb->movecontainer, true, NULL)).interacted) return res;
    return UPDATE_NONE;
}

void layout_lrules_box(LRulesBox lb, Font * fonts){
    CLAY(move_cont_clay_id(lb.movecontainer), move_cont_clay_decl(lb.movecontainer)){
            CLAY_AUTO_ID({ .layout = { .sizing = {CLAY_SIZING_GROW(0), CLAY_SIZING_GROW(0)}
                                     , .layoutDirection = CLAY_LEFT_TO_RIGHT }
                         , .border = {.color = COL_DARK, .width = {0,0,0,0,1}}}){
                CLAY_AUTO_ID({ .layout = { .sizing = { CLAY_SIZING_FIXED(lb.movecontainer.size.x - 16 - 64)
                                                     , CLAY_SIZING_FIXED(lb.movecontainer.size.y - 16)}}}){
                    layout_textbox(lb.textbox, fonts);
                };
                CLAY_AUTO_ID({ .layout = { .sizing = {CLAY_SIZING_GROW(0), CLAY_SIZING_GROW(0)}
                                         , .layoutDirection = CLAY_TOP_TO_BOTTOM
                                         , .childAlignment = {CLAY_ALIGN_X_CENTER, CLAY_ALIGN_Y_TOP}}
                             , .border = {.color = COL_DARK, .width = {0,0,0,0,1}}}){
                    layout_module(*lb.lsys_out);
                    layout_button(lb.button_parse);
                };
            }
            layout_move_container_sizer(lb.movecontainer);
        };
}
