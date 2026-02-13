#include "custom.h"
#include "turtlebox.h"
#include "module.h"
#include "raylib.h"
#include "common.h"
#include "textbox.h"
#include <stdio.h>
#include <stdlib.h>
#define CLAY_IMPLEMENTATION
#include "ui.h"
#include "keys.h"
#include "../clay/clay_renderer_raylib.h"
#include "fonts.h"

void HandleClayErrs(Clay_ErrorData errorData) {
        printf("CLAY ERROR: %s \n", errorData.errorText.chars);
        Clay_ResetMeasureTextCache();
}

clay_ctx init_clay(){
    Font * fonts = get_fonts();

    setup_connection_drawdata();
    uint64_t clayReqMem = Clay_MinMemorySize();
    Clay_Arena clayMem = 
        Clay_CreateArenaWithCapacityAndMemory(clayReqMem, malloc(clayReqMem));

    Clay_Context *ctx = Clay_Initialize(clayMem
            , (Clay_Dimensions) {
                  .width = GetScreenWidth()
                , .height = GetScreenHeight() 
                }
            , (Clay_ErrorHandler) {HandleClayErrs} 
            );

    Clay_SetMeasureTextFunction(Raylib_MeasureText, fonts); 
    return (clay_ctx){
          .ctx = ctx
        , .font_num = 1
        , .fonts = fonts
        , .ced = {0}
        , .num_custom_elems = 0
        , .focus_index = -1
    };
}

void add_textbox(clay_ctx * ctx){
    if (ctx->num_custom_elems >= MAX_CUSTOM_ELEMS) return;
    CustomElementData * elem = malloc(sizeof(CustomElementData));
    if (!elem) return;
    *elem = mk_textbox_elem(2048);
    if (elem->error) {
        free(elem);
        return;
    }
    ctx->ced[ctx->num_custom_elems] = elem;
    ctx->num_custom_elems += 1;
}

void add_turtle_box(clay_ctx * ctx){
    if (ctx->num_custom_elems >= MAX_CUSTOM_ELEMS) return;
    CustomElementData * elem = malloc(sizeof(CustomElementData));
    if (!elem) return;
    *elem = mk_turtlebox_elem();
    if (elem->error) {
        free(elem);
        return;
    }
    ctx->ced[ctx->num_custom_elems] = elem;
    ctx->num_custom_elems += 1;
}

bool update_ui(clay_ctx * ctx){
    if (update_module_connections()) return true;
    if (update_connection_status()) return false;

    if (Clay_PointerOver(Clay_GetElementId(CLAY_STRING("add_textbox")))) {
        if (IsMouseButtonReleased(0)){
            add_textbox(ctx);
            return true;
        }
    }
    if (Clay_PointerOver(Clay_GetElementId(CLAY_STRING("add_turtlebox")))) {
        if (IsMouseButtonReleased(0)){
            add_turtle_box(ctx);
            return true;
        }
    }


    for (int i = 0; i < ctx->num_custom_elems; i++){
        switch (ctx->ced[i]->type) {
            case CUSTOM_ELEM_T_textbox:
                if (update_textbox(&(ctx->ced[i]->textbox), false)) return true;
                break;
            case CUSTOM_ELEM_T_turtle_box:
                if (update_turtlebox(&(ctx->ced[i]->turtlebox))) return true;
                break; 
            default:
                break;
        }
    } 

    KeyboardKey pressed[3] = {GetKeyPressed(), GetKeyPressed(), GetKeyPressed()};
    
    if (!IsKeyDown(KEY_LEFT_CONTROL)) return false;
    unsigned int switch_idx = 0;
    bool shift = false;
    if (pressed[0] == KEY_LEFT_SHIFT 
        || pressed[0] == KEY_RIGHT_SHIFT) {
        switch_idx = 1;
        shift = true;
    } shift = shift 
             || IsKeyDown(KEY_LEFT_SHIFT) 
             || IsKeyDown(KEY_RIGHT_SHIFT);

    switch (pressed[switch_idx]){
        case KEY_ENTER:
            if (ctx->focus_index < ((int)ctx->num_custom_elems - 1)
                && (!shift)){
                ctx->focus_index += 1;
            }else{
                ctx->focus_index = -1;
            }
            return true;
        default: 
            return false;
    }
    return false;
}

Clay_RenderCommandArray mk_layout(clay_ctx ctx){
    Clay_SetCurrentContext(ctx.ctx);
//    Clay_SetDebugModeEnabled(true);
    float width = (float)GetScreenWidth();
    float height = (float)GetScreenHeight();
    Vector2 mouse = GetMousePosition();
    Clay_SetPointerState( (Clay_Vector2) {mouse.x, mouse.y}
                        , IsMouseButtonDown(0)
                        );
    Vector2 scroll = GetMouseWheelMoveV();
    Clay_UpdateScrollContainers( true
                               , (Clay_Vector2) {scroll.x, scroll.y}
                               , GetFrameTime()
                               );
    Clay_SetLayoutDimensions((Clay_Dimensions){.width = width,
                                               .height = height});

    Clay_BeginLayout();

    CLAY(CLAY_ID("SpacingContainer"), { .layout = { .sizing = {CLAY_SIZING_FIXED(width), CLAY_SIZING_FIXED(height)}
                                                  , .layoutDirection = CLAY_LEFT_TO_RIGHT}
                                      , .backgroundColor = COL_TRANSPARENT }) {
        CLAY(CLAY_ID("Sidebar"), { .layout = { .sizing = {CLAY_SIZING_FIXED(24), CLAY_SIZING_GROW(0)}
                                             , .layoutDirection = CLAY_TOP_TO_BOTTOM } 
                                 , .border = { .width = {0,0,0,0,1}, .color = COL_DARK }
                                 , .backgroundColor = COL_LIGHT 
                                 }){
            CLAY(CLAY_ID("add_textbox"), { .layout = { .sizing = {CLAY_SIZING_FIXED(24), CLAY_SIZING_FIXED(24)}
                                                     , .childAlignment = {CLAY_ALIGN_X_CENTER, CLAY_ALIGN_Y_CENTER} }
                                         , .backgroundColor = Clay_Hovered() ? COL_ACCENT : COL_TRANSPARENT }) {
                CLAY_TEXT(CLAY_STRING("+"), CLAY_TEXT_CONFIG({ .fontSize = 16, .fontId = 0, .textColor = {0,0,0,255}, .lineHeight = 16.0 }));
            };
            CLAY(CLAY_ID("add_turtlebox"), { .layout = { .sizing = {CLAY_SIZING_FIXED(24), CLAY_SIZING_FIXED(24)}
                                                     , .childAlignment = {CLAY_ALIGN_X_CENTER, CLAY_ALIGN_Y_CENTER} }
                                         , .backgroundColor = Clay_Hovered() ? COL_ACCENT : COL_TRANSPARENT }) {
                CLAY_TEXT(CLAY_STRING("T"), CLAY_TEXT_CONFIG({ .fontSize = 16, .fontId = 0, .textColor = {0,0,0,255}, .lineHeight = 16.0 }));
            };
        };
        for (int i = 0; i < ctx.num_custom_elems; i++){
            switch (ctx.ced[i]->type) {
                case CUSTOM_ELEM_T_textbox:
                    layout_textbox(ctx.ced[i]->textbox, &ctx.fonts[0]);
                    break;
                case CUSTOM_ELEM_T_turtle_box:
                    layout_turtlebox(ctx.ced[i]->turtlebox, ctx.fonts, "Turtle Box");
                    break;
                default:
                    break;
            }
        }
        }
    return Clay_EndLayout();
}
