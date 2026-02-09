#include "module.h"
#include "raylib.h"
#include "common.h"
#include "textbox.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define CLAY_IMPLEMENTATION
#include "ui.h"
#include "keys.h"
#include "../clay/clay_renderer_raylib.h"

#define CED_MAX_LEN 64

void HandleClayErrs(Clay_ErrorData errorData) {
        printf("CLAY ERROR: %s \n", errorData.errorText.chars);
        Clay_ResetMeasureTextCache();
}

clay_ctx init_clay(){
    Font * fonts = malloc(sizeof(Font));
    setup_connection_drawdata();
    fonts[0]  = LoadFontEx("resources/fonts/roboto_mono/static/RobotoMono-Bold.ttf",48,NULL,0);
    SetTextureFilter(fonts[0].texture, TEXTURE_FILTER_BILINEAR);

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
    //CustomElementData * ced = mk_textbox(8192, "testbox");
   // CustomElementData * ced = malloc(sizeof(CustomElementData) * ced_max_len);

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
    if (ctx->num_custom_elems >= CED_MAX_LEN) return;
    ctx->ced[ctx->num_custom_elems] = mk_textbox_elem(8192);
    ctx->num_custom_elems += 1;
}

bool focus_has_valid_turtle(clay_ctx * ctx){
    if (ctx->focus_index < 0) return false;
    if (ctx->ced[ctx->focus_index]->type 
            != CUSTOM_ELEM_T_textbox) return false;
    if (ctx->ced[ctx->focus_index]->textbox.init_turtle == NULL) return false;
    return true;
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
    for (int i = 0; i < ctx->num_custom_elems; i++){
        switch (ctx->ced[i]->type) {
            case CUSTOM_ELEM_T_textbox:
                if (update_textbox(&(ctx->ced[i]->textbox))) return true;
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
    if (pressed[0] != 0){
        TraceLog(LOG_DEBUG, "key: %s %s %s %d %b", kc_to_rep(pressed[0]),  kc_to_rep(pressed[1]),  kc_to_rep(pressed[2]), switch_idx, shift); 
    }
    switch (pressed[switch_idx]){
        case KEY_ENTER:
            if (ctx->focus_index < ((int)ctx->num_custom_elems - 1)
                && (!shift)){
                ctx->focus_index += 1;
            }else{
                ctx->focus_index = -1;
            }
            return true;
        case KEY_A:  
            if (!focus_has_valid_turtle(ctx)) return false;
            if (shift) {
                ctx->ced[ctx->focus_index]
                    ->textbox.init_turtle->rads += 0.1f;
            }else{
                ctx->ced[ctx->focus_index]
                    ->textbox.init_turtle->rads -= 0.1f;
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
        };
        for (int i = 0; i < ctx.num_custom_elems; i++){
            switch (ctx.ced[i]->type) {
                case CUSTOM_ELEM_T_textbox:
                    layout_textbox(&(ctx.ced[i]->textbox), &ctx.fonts[0], i == ctx.focus_index);
                    break;
                default:
                    break;
            }
        }
        }
    return Clay_EndLayout();
}
