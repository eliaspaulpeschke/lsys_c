#include "drawhook.h"
#include "stdlib.h"
#include <raylib.h>

static DrawHookData * draw_hook_data = NULL;
static unsigned int draw_hook_data_length = 0;
static unsigned int draw_hook_data_capacity = 0;

void realloc_draw_hook_data(){
    if (draw_hook_data_length >= draw_hook_data_capacity){
        unsigned int newlen = draw_hook_data_capacity << 1;
        if (newlen == 0) {
            newlen = 8;
        }
        DrawHookData * temp = realloc(draw_hook_data, newlen * sizeof(DrawHookData));
        if (temp == NULL){
            TraceLog(LOG_FATAL, "No Memory left");
            exit(1);
        }
        draw_hook_data = temp;
        draw_hook_data_capacity = newlen;
    }
}

bool add_draw_hook(void (*draw_function)(void * user_data), void *the_user_data){
    realloc_draw_hook_data();
    draw_hook_data[draw_hook_data_length] = (DrawHookData){ .the_user_data = the_user_data, .draw_function = draw_function};
    draw_hook_data_length += 1;
    return true;
}

void flush_draw_hooks(){
    draw_hook_data_length = 0;
}

void draw_draw_hooks(){
    for (int i = 0; i < draw_hook_data_length; i++){
        draw_hook_data[i].draw_function(
                draw_hook_data[i].the_user_data
        );
    }
}

