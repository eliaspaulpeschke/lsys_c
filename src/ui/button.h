#ifndef BUTTON_H
#define BUTTON_H
#include "stdbool.h"

typedef struct {
    unsigned int clay_id_num;
    char * label;
    void (*on_click)();
}ButtonElem;


ButtonElem mk_button(char * label);
//void free_button();

bool update_button(ButtonElem * btn);
void layout_button(ButtonElem btn);



#endif
