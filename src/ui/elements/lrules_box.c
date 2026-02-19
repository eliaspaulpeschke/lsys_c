#include "lrules_box.h"
#include "../common.h"
#include "../../lsystem/parser/lsystem_parser.h"
#include "raylib.h"

void on_click_parse_btn(void * user_data);

LRulesBox mk_lrules_box(){
    Textbox tb = mk_textbox(2048);
    if (tb.error) return LRULES_BOX_ERR;
    LRulesBox lb = (LRulesBox){
        .textbox = tb 
       , .movecontainer = mk_move_container()
       , .lsys_out = mk_module(MODULE_OUTPUT, MODULE_DATA_TYPE_ruleset)
       , .button_parse = mk_button("parse", on_click_parse_btn)
    };
    lb.button_parse.size.x = 56;
    lb.movecontainer.size = (Vector2){.x = 256, .y=384};
    return lb;
}

void lrules_box_parse(LRulesBox * lb){
    TraceLog(LOG_DEBUG, "Parsing : \"%s\"", lb->textbox.text);
    if (lb->textbox.text == NULL || strlen(lb->textbox.text) == 0) {
        lb->lsys_out.output.valid = false;
        return;
    }
    LRuleset rules = parse_string_to_ruleset(lb->textbox.text, 32);
    if (rules.rules == NULL) {
        lb->lsys_out.output.valid = false;
        return;
    }
    lb->lsys_out.output.ruleset = rules;
    lb->lsys_out.output.valid = true;

}

void on_click_parse_btn(void * user_data){
    LRulesBox * lb = (LRulesBox *)user_data;
    lrules_box_parse(lb);
}

void free_lrules_box(LRulesBox lb){
    free_textbox(lb.textbox);
}

bool update_lrules_box(LRulesBox * lb){
    if (update_module(&lb->lsys_out)) return true;
    if (update_move_container(&lb->movecontainer, true)) return true;
    if (update_textbox(&lb->textbox, false)) return true;
    if (update_button(&lb->button_parse, lb)) return true;
    return false;
}

void layout_lrules_box(LRulesBox lb, Font * fonts){
    CLAY(move_cont_clay_id(lb.movecontainer), move_cont_clay_decl(lb.movecontainer, 0, true)){
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
                    layout_module(lb.lsys_out);
                    layout_button(lb.button_parse);
                };
            }
            layout_move_container_sizer(lb.movecontainer);
        };
}
