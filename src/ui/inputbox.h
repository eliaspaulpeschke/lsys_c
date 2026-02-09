#ifndef INPUTBOX_H
#define INPUTBOX_H
#include <stdbool.h>
#include "raylib.h"
static unsigned int NUM_INPUT_IDS = 0;

typedef struct {
    char * text;
    unsigned int max_len;
    unsigned int cursor;
    unsigned int clay_id_num;
    bool changed;
} inputbox;

inputbox * mk_inputbox(unsigned int max_len /*default 64*/);
void free_inputbox(inputbox * ipb);
bool update_inputbox(inputbox * ipb);
void layout_inputbox(inputbox * ipb, Font * font, bool focus);
#endif
