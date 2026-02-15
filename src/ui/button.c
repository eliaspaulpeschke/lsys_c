#include "button.h"
#include "raylib.h"
#include "../clay/clay.h"

static int NUM_BUTTON_IDS = 0;
static Clay_String ID_STRING = CLAY_STRING("button-element");

ButtonElem mk_button(char *label, void (*on_click)()){
    ButtonElem btn = (ButtonElem){
        .clay_id_num = NUM_BUTTON_IDS
      , .label = label
      , .on_click = on_click
    };
    NUM_BUTTON_IDS++;
    return btn;
}

bool update_button(ButtonElem *btn){
    if (IsMouseButtonReleased(0)){
        if (Clay_PointerOver(Clay_GetElementIdWithIndex(ID_STRING, btn->clay_id_num))){
           btn->on_click();
           return true;
        }
    }
    return false;
}

void layout_button(ButtonElem btn){
    CLAY(CLAY_IDI("button-element", btn.clay_id_num), {}){
    };
}
