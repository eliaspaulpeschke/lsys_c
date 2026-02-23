#include "drawhook.h"
#include "stdlib.h"
#include <raylib.h>
#include <stddef.h>
#include <string.h>

static DrawHookData * draw_hook_data = NULL;
static unsigned int draw_hook_data_length = 0;
static unsigned int draw_hook_data_capacity = 0;

static UpdateHookData * update_hook_data = NULL;
static unsigned int update_hook_data_length = 0;
static unsigned int update_hook_data_capacity = 0;

void realloc_hook_data(unsigned int * length, unsigned int * capacity, void ** ptrptr, size_t data_size){
    if (*length >= *capacity){
        unsigned int newlen = *length << 1;
        if (newlen == 0) {
            newlen = 8;
        }
        void * temp = realloc(*ptrptr, newlen * data_size);
        if (temp == NULL){
            TraceLog(LOG_FATAL, "No Memory left");
            exit(1);
        }
        *ptrptr = temp;
        *capacity = newlen;
    }
}

void remove_hook(unsigned int * length, void * ptrptr, size_t data_size, int hook_handle){
    if (hook_handle < 0) return;
    unsigned int idx = (unsigned int)hook_handle;
    if (idx >= *length) return;
    char * editptr = (char *)ptrptr;
    memmove(editptr + (idx * data_size), editptr + ((idx+1) * data_size), *length - idx - 1);
    memset(ptrptr + (*length - 2), 0, data_size);
    *length -= 1;
    return;
}

void remove_update_hook(unsigned int update_hook_handle){
    remove_hook(&update_hook_data_length, update_hook_data, sizeof(UpdateHookData), update_hook_handle);
}

unsigned int add_draw_hook(void (*draw_function)(void * user_data), void *the_user_data){
    realloc_hook_data(&draw_hook_data_length, &draw_hook_data_capacity,(void **)&draw_hook_data, sizeof(DrawHookData));
    draw_hook_data[draw_hook_data_length] = (DrawHookData){ .the_user_data = the_user_data, .draw_function = draw_function};
    draw_hook_data_length += 1;
    return draw_hook_data_length -1;
}

unsigned int add_update_hook(void (*update_function)(void * user_data), void *the_user_data){
    realloc_hook_data(&update_hook_data_length, &update_hook_data_capacity,(void **)&update_hook_data, sizeof(UpdateHookData));
    update_hook_data[update_hook_data_length] = (UpdateHookData){ .the_user_data = the_user_data, .update_function= update_function};
    update_hook_data_length += 1;
    return update_hook_data_length - 1;
}

void flush_draw_hooks(){
    draw_hook_data_length = 0;
}
void flush_update_hooks(){
    update_hook_data_length = 0;
}

void draw_draw_hooks(){
    for (int i = 0; i < draw_hook_data_length; i++){
        draw_hook_data[i].draw_function(
                draw_hook_data[i].the_user_data
        );
    }
}

void update_update_hooks(){
    for (int i = 0; i < update_hook_data_length; i++){
        update_hook_data[i].update_function(
                update_hook_data[i].the_user_data
        );
    }
}


