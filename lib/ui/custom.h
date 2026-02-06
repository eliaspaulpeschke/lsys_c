#ifndef CUSTOM_H
#define CUSTOM_H
#include "../clay.h"
#include "./textbox.h"
#include "raylib.h"
#include <stdlib.h>
#include <string.h>

typedef enum {
    CUSTOM_ELEM_T_textbox
} CustomElementType;

typedef struct {
    CustomElementType type;
    union {
        textbox textbox;
    };
} CustomElementData;

CustomElementData * mk_textbox(uint max_len, uint id){
    CustomElementData * ced = 
        malloc(sizeof(CustomElementData));
    *ced = (CustomElementData) {
                   .type = CUSTOM_ELEM_T_textbox,
                   .textbox =
                       (textbox){ .text = malloc(2048) 
                       , .changed = false
                       , .lenText = 2048
                       , .bufA = malloc(1024)
                       , .bufB = malloc(1024)
                       , .lenA = 1024
                       , .lenB = 1024
                       , .posA = 0
                       , .posB = 1023
                       , .max_len = max_len
                       , .size = (Vector2){250, 250}
                       , .pos = (Vector2){300, 200}
                       , .clay_id_num = id
                       , .lsystem = NULL
                       , .generated = NULL
                       , .init_turtle = NULL 
                       }
                   };
    ced->textbox.init_turtle = mk_base_turtle();
    memset(ced->textbox.bufA, '\0', ced->textbox.lenA);
    memset(ced->textbox.bufB, '\0', ced->textbox.lenB);
    memset(ced->textbox.text, '\0', ced->textbox.lenText);
    return ced;
}
#endif
