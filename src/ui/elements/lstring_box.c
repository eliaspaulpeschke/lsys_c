#include "lstring_box.h"
#include "../common.h"
#include "../../lsystem/parser/lsystem_parser.h"
#include "raylib.h"

static void on_click_parse_btn(void * user_data);

LStringBox mk_lstring_box(){
    Textbox tb = mk_textbox(2048);
    if (tb.error) return LSTRING_BOX_ERR;
    Module * lstring_out = mk_module(MODULE_OUTPUT, MODULE_DATA_TYPE_lstring);
    if (lstring_out->type == MODULE_NONE) {
        free_textbox(tb);
        return LSTRING_BOX_ERR;
    }
    Module * lstring_in = mk_module(MODULE_INPUT, MODULE_DATA_TYPE_lstring);
    if (lstring_in->type == MODULE_NONE) {
        free_textbox(tb);
        free(lstring_out);
        return LSTRING_BOX_ERR;
    }

    LStringBox lb = (LStringBox){
        .textbox = tb 
       , .movecontainer = mk_move_container(NULL, NULL)
       , .lstring_out = lstring_out
       , .lstring_in = lstring_in
       , .button_parse = mk_button("update", on_click_parse_btn)
    };
    lb.button_parse.size.x = 56;
    lb.movecontainer.size = (Vector2){.x = 256, .y=384};
    return lb;
}

void lstring_box_parse(LStringBox * lb){
    if (lb->lstring_in->input.connection != NULL) return;
    TraceLog(LOG_DEBUG, "Parsing : \"%s\"", lb->textbox.text);
    if (lb->textbox.text == NULL || strlen(lb->textbox.text) == 0) {
        lb->lstring_out->output.valid = false;
        return;
    }
    LString lstring = parse_lstring(lb->textbox.text);
    if (lstring.content == NULL 
        || lstring.capacity <= 0 
        || lstring.length <= 0) {
        lb->lstring_out->output.valid = false;
        return;
    }
    lb->lstring_out->output.lstring = lstring;
    lb->lstring_out->output.valid = true;

}

static void on_click_parse_btn(void * user_data){
    LStringBox * lb = (LStringBox *)user_data;
    lstring_box_parse(lb);
}

void free_lstring_box(LStringBox lb){
    free_textbox(lb.textbox);
    free(lb.lstring_in);
    free(lb.lstring_out);
}

bool update_lstring_box(LStringBox * lb){

    if (update_module(lb->lstring_out)) return true;
    if (update_module(lb->lstring_in)) return true;
    if (update_move_container(&lb->movecontainer, true, NULL)) return true;
    if (update_button(&lb->button_parse, lb)) return true;
    if (lb->lstring_in->input.connection != NULL) {
        if (!lb->lstring_in->input.connection->output.valid) 
                                                    return false;
        char * ls = lstring_to_string(&lb->lstring_in->input.connection->output.lstring);
        if (ls != NULL){
            textbox_set_text(&lb->textbox, ls);
            free(ls);
        }
    } else {
        if (update_textbox(&lb->textbox, false)) return true;
    }
    return false;
}

void layout_lstring_box(LStringBox lb, Font * fonts){
    CLAY(move_cont_clay_id(lb.movecontainer), move_cont_clay_decl(lb.movecontainer, 0, true)){
            CLAY_AUTO_ID({ .layout = { .sizing = {CLAY_SIZING_GROW(0), CLAY_SIZING_GROW(0)}
                                     , .layoutDirection = CLAY_LEFT_TO_RIGHT }
                         , .border = {.color = COL_DARK, .width = {0,0,0,0,1}}}){
                CLAY_AUTO_ID({ .layout = { .sizing = {CLAY_SIZING_GROW(0), CLAY_SIZING_GROW(0)}
                                         , .layoutDirection = CLAY_TOP_TO_BOTTOM
                                         , .childAlignment = {CLAY_ALIGN_X_CENTER, CLAY_ALIGN_Y_TOP}}
                             , .border = {.color = COL_DARK, .width = {0,0,0,0,1}}}){
                    layout_module(*lb.lstring_in);
                };
                CLAY_AUTO_ID({ .layout = { .sizing = { CLAY_SIZING_FIXED(lb.movecontainer.size.x - 16 - 64 - 64)
                                                     , CLAY_SIZING_FIXED(lb.movecontainer.size.y - 16)}}}){
                    layout_textbox(lb.textbox, fonts);
                };
                CLAY_AUTO_ID({ .layout = { .sizing = {CLAY_SIZING_GROW(0), CLAY_SIZING_GROW(0)}
                                         , .layoutDirection = CLAY_TOP_TO_BOTTOM
                                         , .childAlignment = {CLAY_ALIGN_X_CENTER, CLAY_ALIGN_Y_TOP}}
                             , .border = {.color = COL_DARK, .width = {0,0,0,0,1}}}){
                    layout_module(*lb.lstring_out);
                    layout_button(lb.button_parse);
                };
            }
            layout_move_container_sizer(lb.movecontainer);
        };
}
