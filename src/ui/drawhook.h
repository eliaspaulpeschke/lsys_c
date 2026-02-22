#ifndef DRAWHOOK_H
#define DRAWHOOK_H
#include "stdbool.h"

typedef struct {
    void * the_user_data;
    void (*draw_function)(void * user_data); 
} DrawHookData;

bool add_draw_hook(void (*draw_function)(void * user_data), void * the_user_data);
void flush_draw_hooks();
void draw_draw_hooks();

#endif
