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

CustomElementData * mk_textbox(uint max_len, char * id){
    char * sizer_id = malloc(strlen(id) + 7);
    strcpy(sizer_id, id);
    strcpy(sizer_id + strlen(id), "-sizer");
    char * button_id = malloc(strlen(id) + 9);
    strcpy(button_id, id);
    strcpy(button_id + strlen(id), "-button1");

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
                       , .clay_id = (Clay_String){.isStaticallyAllocated = false, .length=strlen(id)-1, .chars=id}
                       , .sizer_id = (Clay_String){.isStaticallyAllocated = false, .length=strlen(sizer_id) -1, .chars=sizer_id}
                       , .button_ids = { (Clay_String){.isStaticallyAllocated = false, .length=strlen(button_id) -1, .chars=button_id} }
                       , .lsystem = NULL
                       }
                   };
    memset(ced->textbox.bufA, '\0', ced->textbox.lenA);
    memset(ced->textbox.bufB, '\0', ced->textbox.lenB);
    memset(ced->textbox.text, '\0', ced->textbox.lenText);
    return ced;
}
#endif
