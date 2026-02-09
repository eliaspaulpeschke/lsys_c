#ifndef TEXTBOX_H
#define TEXTBOX_H
#include <stdbool.h>
#include "raylib.h"
#include "../lsystem/lsystem.h"
#include "../turtle/turtle.h"
#include "inputbox.h"
#include "module.h"

typedef struct {
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
    Vector2 size;
    Vector2 pos;
    unsigned int clay_id_num;
    Lsystem * lsystem;
    char * generated;
    Turtle * init_turtle;
    inputbox * title;
    Module module1;
    Module module2;
} textbox;

textbox mk_textbox(unsigned int max_len);
void free_textbox(textbox * tb);
void textbox_update_text(textbox * tb);
void textbox_update_lsystem(textbox * tb);
void textbox_generate(textbox * tb);
bool update_textbox(textbox * tb);
void layout_textbox(textbox * tb, Font * font, bool focused);
#endif

