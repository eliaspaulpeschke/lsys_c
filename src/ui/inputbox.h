#ifndef INPUTBOX_H
#define INPUTBOX_H
#include <stdbool.h>
#include <stdlib.h>
#include "raylib.h"
static unsigned int NUM_INPUT_IDS = 0;

typedef struct {
    bool error;
    char * text;
    unsigned int max_len;
    unsigned int cursor;
    unsigned int clay_id_num;
    bool changed;
} Inputbox;

#define ERR_INPUTBOX (Inputbox){true, NULL, 0, 0, 0, 0}

void set_inputbox_text(Inputbox ipb, char * text);

Inputbox mk_inputbox(unsigned int max_len /*default 64*/);
void free_inputbox(Inputbox ipb);
bool update_inputbox(Inputbox * ipb);
void layout_inputbox(Inputbox ipb, Font * font, bool focus, char * label);
#endif
