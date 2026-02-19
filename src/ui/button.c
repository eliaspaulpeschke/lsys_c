#include "button.h"
#include "raylib.h"
#include "../clay/clay.h"
#include "common.h"

static int NUM_BUTTON_IDS = 0;
static Clay_String ID_STRING = CLAY_STRING("button-element");

ButtonElem mk_button(char *label, void (*on_click)(void * user_data)){
    if (label == NULL) return ERR_BUTTON;
    if (on_click == NULL) return ERR_BUTTON;
    ButtonElem btn = (ButtonElem){
        .error = false
      , .clay_id_num = NUM_BUTTON_IDS
      , .label = label
      , .on_click = on_click
      , .size = (Vector2){.x = 72, .y=32}
    };
    NUM_BUTTON_IDS++;
    return btn;
}

bool update_button(ButtonElem *btn, void * user_data){
    if (IsMouseButtonReleased(0)){
        if (Clay_PointerOver(Clay_GetElementIdWithIndex(ID_STRING, btn->clay_id_num))){
           btn->on_click(user_data);
           return true;
        }
    }
    return false;
}


void layout_button(ButtonElem btn){
    CLAY(CLAY_IDI("button-element", btn.clay_id_num), {
            .layout = { .sizing = { CLAY_SIZING_FIXED(btn.size.x)
                                  , CLAY_SIZING_FIXED(btn.size.y)}
                      , .childAlignment = { CLAY_ALIGN_X_CENTER
                                          , CLAY_ALIGN_Y_CENTER }
                      , .padding = CLAY_PADDING_ALL(4)
                      }
            }){
            CLAY_AUTO_ID({ .layout = { .sizing = { CLAY_SIZING_FIXED(btn.size.x - 8), CLAY_SIZING_FIXED(btn.size.y - 8)}
                                     , .padding = CLAY_PADDING_ALL(4) 
                                     , .childAlignment = { CLAY_ALIGN_X_CENTER, CLAY_ALIGN_Y_CENTER }
                                     }
                         , .border = { .color = Clay_Hovered() ? COL_ACCENT : COL_DARK, .width = {1,1,1,1,0} }
            }){
            TEXT_STANDARD(btn.label);
            };
    };
}
