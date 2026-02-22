#include "apply_box.h"
#include "../common.h"
#include <raylib.h>
#include <string.h>

void apply_on_click(void * user_data){
    ApplyBox * ab = (ApplyBox *) user_data;
    if (ab->lstring_in->input.connection == NULL) return; 
    if (ab->rules_in->input.connection == NULL) return;
    if (!ab->lstring_in->input.connection->output.valid) {
        ab->lstring_out->output.valid = false;
        return; 
    }
    if (!ab->rules_in->input.connection->output.valid){
        ab->lstring_out->output.valid = false;
        return; 
    }
    LString in_str = ab->lstring_in->input.connection->output.lstring;
    LRuleset rules = ab->rules_in->input.connection->output.ruleset; 
    LString * out_str = apply_rules_n(rules, &in_str, ab->times);
    ab->lstring_out->output.lstring = *out_str;
    free(out_str);
    ab->lstring_out->output.valid = true;
}

ApplyBox mk_applybox(){
    ButtonElem btn_apply = mk_button("apply", apply_on_click);
    if (btn_apply.error == true) return ERR_APPLYBOX;
    ValueBox valuebox = mk_valuebox(VALUEBOX_TYPE_int, false);
    if (valuebox.error == true) return ERR_APPLYBOX;
    valuebox.int_value.min = 0;
    Module * lstring_in = mk_module(MODULE_INPUT, MODULE_DATA_TYPE_lstring);
    if (lstring_in->type == MODULE_NONE) {
        free_valuebox(valuebox);
        return ERR_APPLYBOX;
    }
    Module * lstring_out = mk_module(MODULE_OUTPUT, MODULE_DATA_TYPE_lstring);
    if (lstring_out->type == MODULE_NONE) {
        free_valuebox(valuebox);
        free(lstring_in);
        return ERR_APPLYBOX;
    }
    Module * rules_in = mk_module(MODULE_INPUT, MODULE_DATA_TYPE_ruleset);
    if (rules_in->type == MODULE_NONE) {
        free_valuebox(valuebox);
        free(lstring_in);
        free(lstring_out);
        return ERR_APPLYBOX;
    }
    Move_container container = mk_move_container(NULL, NULL);
    return (ApplyBox) {
          .error = false
        , .times = 0
        , .valuebox = valuebox
        , .btn_apply = btn_apply
        , .lstring_in = lstring_in
        , .rules_in = rules_in
        , .lstring_out = lstring_out
        , .container = container
    };
}

void free_applybox(ApplyBox ab){
    free_valuebox(ab.valuebox);
    free(ab.lstring_out);
    free(ab.lstring_in);
    free(ab.rules_in);
}

bool update_applybox(ApplyBox *ab){
   if (ab->valuebox.changed) {
       ab->times = ab->valuebox.int_value.value;
       ab->valuebox.changed = false;
   }
   if (update_module(ab->lstring_in)) return true;
   if (update_module(ab->lstring_out)) return true;
   if (update_module(ab->rules_in)) return true;
   if (update_valuebox(&ab->valuebox)) return true;
   if (update_button(&ab->btn_apply, ab)) return true;
   if (update_move_container(&ab->container, true, NULL)) return true;
   return false;
}

void layout_applybox(ApplyBox ab, Font * fonts){
    CLAY(move_cont_clay_id(ab.container), move_cont_clay_decl(ab.container, 8, false)){
        CLAY_AUTO_ID({.layout = { SIZE_GROW_XY(0)
                                , LAYOUT_LR 
                                }
                     , .border = {.color = COL_DARK, .width = {0, 0, 0, 0, 1} }
                     }){
            CLAY_AUTO_ID({.layout = { SIZE_COLUMN(32)
                                    , LAYOUT_TB
                                    , .childAlignment = {CLAY_ALIGN_X_CENTER, CLAY_ALIGN_Y_TOP}}
                         }){
                layout_module(*ab.lstring_in);
                layout_module(*ab.rules_in);
            }
            CLAY_AUTO_ID({.layout = { SIZE_GROW_XY(0)
                                    , LAYOUT_TB
                                    , .padding = { .left = 8
                                                 , .right = 8
                                                 , .top = 4
                                                 , .bottom = 0
                                                 } 
                                    , .childAlignment = {CLAY_ALIGN_X_CENTER, CLAY_ALIGN_Y_TOP}
                                    }
                                
                         }){
                layout_valuebox(ab.valuebox, fonts, "n: ");
                CLAY_AUTO_ID({.layout = { SIZE_GROW_XY(0)
                                        , LAYOUT_TB 
                                        , .childAlignment = {CLAY_ALIGN_X_CENTER, CLAY_ALIGN_Y_BOTTOM} 
                                        }
                             }){
                    layout_button(ab.btn_apply);
                };
            };
            CLAY_AUTO_ID({.layout = { SIZE_COLUMN(32)
                                    , LAYOUT_TB
                                    , .childAlignment = {CLAY_ALIGN_X_CENTER, CLAY_ALIGN_Y_TOP}
                                    }
                         }){
                layout_module(*ab.lstring_out);
            }
        }
    }
}
