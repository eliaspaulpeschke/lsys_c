#ifndef DRAWHOOK_H
#define DRAWHOOK_H
#include "stdbool.h"

typedef struct {
    void * the_user_data;
    void (*draw_function)(void * user_data); 
} DrawHookData;

typedef struct {
    void * the_user_data;
    void (*update_function)(void * user_data); 
} UpdateHookData;

unsigned int add_draw_hook(void (*draw_function)(void * user_data), void * the_user_data);
unsigned int add_update_hook(void (*update_function)(void * user_data), void * the_user_data);

void flush_draw_hooks();
void flush_update_hooks();

void remove_update_hook(unsigned int update_hook_handle);

void draw_draw_hooks();
void update_update_hooks();


#endif
