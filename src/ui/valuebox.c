#include "valuebox.h"
#include "button.h"
#include "inputbox.h"
#include "move_container.h"
#include <float.h>
#include <limits.h>
#include <errno.h>
#include <raylib.h>
#include <string.h>
#include "common.h"

static unsigned int NUM_VALUEBOXES = 0;

void on_click_btn_plus(void * user_data);
void on_click_btn_minus(void * user_data);

ValueBox mk_valuebox(VALUEBOX_TYPE type, bool wrap){
    ButtonElem btn_plus = mk_button("+", on_click_btn_plus);
    ButtonElem btn_minus = mk_button("-", on_click_btn_minus);
    Inputbox inputbox = mk_inputbox(32);
    if (inputbox.error) return ERR_VALUEBOX;
    ValueBox vb = (ValueBox){ .error = false
                            , .active = false
                            , .type = type 
                            , .clay_id_num = NUM_VALUEBOXES
                            , .btn_plus = btn_plus
                            , .btn_minus = btn_minus
                            , .inputbox = inputbox
                            , .wrap = wrap
                            , .changed = false
                            };
    NUM_VALUEBOXES += 1;
    switch (type){
        case VALUEBOX_TYPE_none:
            return ERR_VALUEBOX;
        case VALUEBOX_TYPE_int:
            vb.int_value = (ValueBoxIntValue) 
                             {0, INT_MAX, INT_MIN, 1};
            sprintf_inputbox_text(&vb.inputbox, "0"); 
            break;
        case VALUEBOX_TYPE_float:
            vb.float_value = (ValueBoxFloatValue) 
                             {0.0f, FLT_MAX, -FLT_MAX, 1.0f};
            sprintf_inputbox_text(&vb.inputbox, "0.0"); 
            break;
        case VALUEBOX_TYPE_double:
            vb.double_value = (ValueBoxDoubleValue) 
                             {0.0, DBL_MAX, -DBL_MAX, 1.0};
            sprintf_inputbox_text(&vb.inputbox, "0.0"); 
            break;
        default:
            return ERR_VALUEBOX;
    };

    return vb;
}

void free_valuebox(ValueBox vb){
    free_inputbox(vb.inputbox);
}


void value_from_text(ValueBox *vb){
        int int_val;
        float float_val;
        double double_val;
        char * endptr;
        errno = 0;
        switch(vb->type){
            case VALUEBOX_TYPE_none:
                return;
            case VALUEBOX_TYPE_int:
                int_val = strtol(vb->inputbox.text, &endptr, 10); 
                if (endptr == vb->inputbox.text || errno != 0 ){
                    sprintf_inputbox_text(&vb->inputbox, "%d", vb->int_value.value); 
                } else {
                    if (int_val > vb->int_value.max) vb->int_value.value = vb->int_value.max;
                    if (int_val < vb->int_value.min) vb->int_value.value = vb->int_value.min;
                    vb->int_value.value = int_val;
                    sprintf_inputbox_text(&vb->inputbox, "%d", vb->int_value.value); 
                }
                vb->changed = true;
                return;
            case VALUEBOX_TYPE_float:
                float_val = strtof(vb->inputbox.text, &endptr); 
                if (endptr == vb->inputbox.text || errno != 0 ){
                    sprintf_inputbox_text(&vb->inputbox, "%f", vb->float_value.value); 
                } else {
                    if (float_val > vb->float_value.max) vb->float_value.value = vb->float_value.max;
                    if (float_val < vb->float_value.min) vb->float_value.value = vb->float_value.min;
                    vb->float_value.value = float_val;
                    sprintf_inputbox_text(&vb->inputbox, "%f", vb->float_value.value); 
                }
                vb->changed = true;
                return;
            case VALUEBOX_TYPE_double:
                double_val = strtod(vb->inputbox.text, &endptr); 
                if (endptr == vb->inputbox.text || errno != 0 ){
                    sprintf_inputbox_text(&vb->inputbox, "%f", vb->double_value.value); 
                } else {
                    if (double_val > vb->double_value.max) vb->double_value.value = vb->double_value.max;
                    if (double_val < vb->double_value.min) vb->double_value.value = vb->double_value.min;
                    vb->double_value.value = double_val;
                    sprintf_inputbox_text(&vb->inputbox, "%f", vb->double_value.value); 
                }
                vb->changed = true;
                return;
            default:
                return;
        }

}

void on_click_btn_plus(void * user_data){
    ValueBox *vb = (ValueBox *) user_data;
    value_from_text(vb);
    switch (vb->type) {
        case VALUEBOX_TYPE_none:
            return;
        case VALUEBOX_TYPE_int:
            vb->int_value.value += vb->int_value.step;
            if (vb->int_value.value > vb->int_value.max) {
                if (vb->wrap) { vb->int_value.value = vb->int_value.min; }
                if (!vb->wrap) { vb->int_value.value = vb->int_value.max; }
            }
            sprintf_inputbox_text(&vb->inputbox, "%d", vb->int_value.value); 
            vb->changed = true;
            return;
        case VALUEBOX_TYPE_float:
            vb->float_value.value += vb->float_value.step;
            if (vb->float_value.value > vb->float_value.max){
                if (vb->wrap) { vb->float_value.value = vb->float_value.min; }
                if (!vb->wrap) { vb->float_value.value = vb->float_value.max; }
            }
            sprintf_inputbox_text(&vb->inputbox, "%f", vb->float_value.value); 
            vb->changed = true;
            return;
        case VALUEBOX_TYPE_double:
            vb->double_value.value += vb->double_value.step;
            if (vb->double_value.value > vb->double_value.max){
                if (vb->wrap) { vb->double_value.value = vb->double_value.min; }
                if (!vb->wrap) { vb->double_value.value = vb->double_value.max; }
            }
            sprintf_inputbox_text(&vb->inputbox, "%f", vb->double_value.value); 
            vb->changed = true;
            return;
        default:
            return;
    }
}

void on_click_btn_minus(void * user_data){
    ValueBox *vb = (ValueBox *) user_data;
    value_from_text(vb);
    switch (vb->type) {
        case VALUEBOX_TYPE_none:
            return;
        case VALUEBOX_TYPE_int:
            vb->int_value.value -= vb->int_value.step;
            if (vb->int_value.value < vb->int_value.min) {
                if (vb->wrap) { vb->int_value.value = vb->int_value.max; }
                if (!vb->wrap) { vb->int_value.value = vb->int_value.min; }
            }
            sprintf_inputbox_text(&vb->inputbox, "%d", vb->int_value.value); 
            vb->changed = true;
            return;
        case VALUEBOX_TYPE_float:
            vb->float_value.value -= vb->float_value.step;
            if (vb->float_value.value < vb->float_value.min){
                if (vb->wrap) { vb->float_value.value = vb->float_value.max; }
                if (!vb->wrap) { vb->float_value.value = vb->float_value.min; }
            }
            sprintf_inputbox_text(&vb->inputbox, "%f", vb->float_value.value); 
            vb->changed = true;
            return;
        case VALUEBOX_TYPE_double:
            vb->double_value.value -= vb->double_value.step; 
            if (vb->double_value.value < vb->double_value.min){
                if (vb->wrap) { vb->double_value.value = vb->double_value.max; }
                if (!vb->wrap) { vb->double_value.value = vb->double_value.min; }
            }
            sprintf_inputbox_text(&vb->inputbox, "%f", vb->double_value.value); 
            vb->changed = true;
            return;
        default:
            return;
    }
}


bool update_valuebox(ValueBox *vb){
    bool hover = Clay_PointerOver(Clay_GetElementIdWithIndex(CLAY_STRING("valuebox"), vb->clay_id_num));
    if (hover) {
//        vb->active = true;
        if (update_button(&vb->btn_plus, vb)) return true;
        if (update_button(&vb->btn_minus, vb)) return true;
        if (update_inputbox(&vb->inputbox)) {
            value_from_text(vb);
            return true;
        }
      }
 /*     else if (vb->active == true) {
        vb->active = false;
        if (!vb->inputbox.changed) return false;
        vb->inputbox.changed = false;
        vb->changed = true;
        value_from_text(vb);
    } */
    return false;
}

Clay_TextElementConfig * btn_text(Clay_Color col) {
    return CLAY_TEXT_CONFIG(
        { .fontSize = 12
        , .fontId = 0
        , .textColor = col
        , .lineHeight = 12.0 });
}

void layout_valuebox(ValueBox vb, Font * fonts, char * label){
    CLAY( CLAY_IDI("valuebox", vb.clay_id_num)
        , { .layout = { .sizing = { CLAY_SIZING_FIT(0)
                                  , CLAY_SIZING_FIT(0) }
                      , .layoutDirection = CLAY_LEFT_TO_RIGHT 
                      , .childAlignment = { CLAY_ALIGN_X_CENTER
                                          , CLAY_ALIGN_Y_CENTER }
                      }}){
        layout_inputbox(vb.inputbox, fonts, vb.active, false, label);
        CLAY_AUTO_ID({ .layout = { .sizing = { CLAY_SIZING_FIXED(16)
                                             , CLAY_SIZING_FIXED(24) }
                                 , .layoutDirection =
                                       CLAY_TOP_TO_BOTTOM
                                 , .childAlignment = 
                                       { CLAY_ALIGN_X_CENTER
                                       , CLAY_ALIGN_Y_CENTER 
                                       }
                                 , .padding = { .left = 4
                                              , .bottom = 0
                                              , .right = 0
                                              , .top = 0 }
                                  }}){


            CLAY( CLAY_IDI( "button-element"
                          , vb.btn_plus.clay_id_num)
                , { .layout = { .sizing = { CLAY_SIZING_GROW(0)
                                          , CLAY_SIZING_FIXED(12) 
                                          }}}){
                       CLAY_TEXT(CLAYIFY(vb.btn_plus.label), btn_text(Clay_Hovered() ? COL_ACCENT : COL_DARK));
                };

            CLAY( CLAY_IDI( "button-element"
                          , vb.btn_minus.clay_id_num)
                , { .layout = { .sizing = { CLAY_SIZING_GROW(0)
                                          , CLAY_SIZING_FIXED(12) 
                                          }}}){
                       CLAY_TEXT(CLAYIFY(vb.btn_minus.label), btn_text(Clay_Hovered() ? COL_ACCENT : COL_DARK));
                };
        };
    }
}
