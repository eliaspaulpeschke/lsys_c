#include "apply_box.h"

void apply_on_click(void * user_data){
    ApplyBox * ab = (ApplyBox *) user_data;
    if (ab->lstring_in.input.connection == NULL) return; 
    if (ab->rules_in.input.connection == NULL) return;
    if (!ab->lstring_in.input.connection->output.valid) return; 
    if (!ab->rules_in.input.connection->output.valid) return;
    LString lstr = ab->lstring_in.input.connection->output.lstring;
    LRuleset rules = ab->rules_in.input.connection->output.ruleset; 
    apply_rules_n(rules, &lstr, ab->times);
    ab->lstring_out.output.lstring = lstr;
    ab->lstring_out.output.valid = true;
}

ApplyBox mk_applybox(){
    ButtonElem btn_apply = mk_button("apply", apply_on_click);
    if (btn_apply.error == true) return ERR_APPLYBOX;
    ValueBox valuebox = mk_valuebox(VALUEBOX_TYPE_int, false);
    if (valuebox.error == true) return ERR_APPLYBOX;
    valuebox.int_value.min = 0;
    Module lstring_in = mk_module(MODULE_INPUT, MODULE_DATA_TYPE_lstring);
    if (lstring_in.type == MODULE_NONE) {
        free_valuebox(valuebox);
        return ERR_APPLYBOX;
    }
    Module lstring_out = mk_module(MODULE_OUTPUT, MODULE_DATA_TYPE_lstring);
    if (lstring_out.type == MODULE_NONE) {
        free_valuebox(valuebox);
        return ERR_APPLYBOX;
    }
    Module rules_in = mk_module(MODULE_INPUT, MODULE_DATA_TYPE_ruleset);
    if (rules_in.type == MODULE_NONE) {
        free_valuebox(valuebox);
        return ERR_APPLYBOX;
    }
    Move_container container = mk_move_container();
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
}

bool update_applybox(ApplyBox *ab){
   if (ab->valuebox.changed) {
       ab->times = ab->valuebox.int_value.value;
       ab->valuebox.changed = false;
   }
   if (update_module(&ab->lstring_in)) return true;
   if (update_module(&ab->lstring_out)) return true;
   if (update_module(&ab->rules_in)) return true;
   if (update_valuebox(&ab->valuebox)) return true;
   if (update_button(&ab->btn_apply, ab)) return true;
   if (update_move_container(&ab->container, true)) return true;
   return false;
}

void layout_applybox(ApplyBox ab, Font * fonts){
    CLAY(move_cont_clay_id(ab.container), move_cont_clay_decl(ab.container, 8, false)){
        layout_valuebox(ab.valuebox, fonts, "n: ");
        layout_button(ab.btn_apply);
        layout_module(ab.lstring_in);
        layout_module(ab.rules_in);
        layout_module(ab.lstring_out);
    }
}
