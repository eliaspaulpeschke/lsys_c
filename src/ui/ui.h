#ifndef UI_H
#define UI_H
#include "../clay/clay.h"
#include <stdbool.h>
#include <raylib.h>
#include "common.h"
#include "custom.h"

#define MAX_CUSTOM_ELEMS 64

typedef struct {
    Clay_Context * ctx;
    unsigned int font_num;
    Font * fonts; 
    CustomElementData * ced[MAX_CUSTOM_ELEMS];
    int focus_index;
    unsigned int num_custom_elems;
} clay_ctx;

clay_ctx init_clay();
UpdateReturnValue update_ui(clay_ctx * ctx);
Clay_RenderCommandArray mk_layout(clay_ctx ctx);

#endif
