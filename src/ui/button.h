#ifndef BUTTON_H
#define BUTTON_H
#include "raylib.h"
#include "stdbool.h"


typedef struct {
    unsigned int clay_id_num;
    char * label;
    void (*on_click)(void * user_data);
    Vector2 size;
}ButtonElem;


ButtonElem mk_button(char * label, void (*on_click)(void * user_data));
//void free_button();

bool update_button(ButtonElem * btn, void * user_data);
void layout_button(ButtonElem btn);



#endif
