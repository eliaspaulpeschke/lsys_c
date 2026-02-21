#ifndef TEXTBOX_H
#define TEXTBOX_H
#include <stdbool.h>
#include "raylib.h"

#define ERR_TEXTBOX (Textbox){true,0,0,0,0,0,0,0,0,0,0,0,0}

typedef struct {
    bool error;
    char * text;
    bool changed;
    unsigned int lenTextBuf;
    unsigned int lenText;
    char * bufA;
    char * bufB;
    unsigned int lenA;
    unsigned int lenB;
    unsigned int posA;
    unsigned int posB; //used backwards, start at lenB-1
    unsigned int max_len;
    unsigned int clay_id_num;
} Textbox;

Textbox mk_textbox(unsigned int max_len);
void textbox_set_text(Textbox * tb, char * text);
void free_textbox(Textbox tb);
void textbox_update_text(Textbox * tb);
bool update_textbox(Textbox * tb, bool focused_anyway);
void layout_textbox(Textbox tb, Font * font);
#endif

