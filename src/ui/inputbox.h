#ifndef INPUTBOX_H
#define INPUTBOX_H
#include <stdbool.h>
#include <stdlib.h>
#include "common.h"
#include "raylib.h"

typedef struct {
    bool error;
    char * text;
    unsigned int max_len;
    unsigned int cursor;
    unsigned int clay_id_num;
    bool changed;
} Inputbox;

#define ERR_INPUTBOX (Inputbox){true, NULL, 0, 0, 0, 0}
void sprintf_inputbox_text(Inputbox * ipb, const char * format, ...);
void set_inputbox_text(Inputbox * ipb, char * text);

Inputbox mk_inputbox(unsigned int max_len /*default 64*/);
void free_inputbox(Inputbox ipb);
UpdateReturnValue update_inputbox(Inputbox * ipb);
void layout_inputbox(Inputbox ipb, Font * font, bool focus, bool padd, char * label);
#endif
