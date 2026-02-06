#ifndef UI_H
#define UI_H
#include "raylib.h"
#include "colors.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define CLAY_IMPLEMENTATION
#include "../clay.h"
#include "custom.h"
#include "../keys.h"
#include "../clay_renderer_raylib.c"

#define CED_MAX_LEN 64

typedef struct {
    Clay_Context * ctx;
    uint font_num;
    Font * fonts; 
    CustomElementData * ced[64];
    CustomElementData * focused;
    uint num_custom_elems;
} clay_ctx;

void HandleClayErrs(Clay_ErrorData errorData) {
        printf("CLAY ERROR: %s \n", errorData.errorText.chars);
}

clay_ctx init_clay(){
    Font * fonts = malloc(sizeof(Font));
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
        , .focused = NULL
        , .num_custom_elems = 0
    };
}

void add_textbox(clay_ctx * ctx){
    if (ctx->num_custom_elems >= CED_MAX_LEN) return;
    ctx->ced[ctx->num_custom_elems] = mk_textbox(8192, ctx->num_custom_elems);
    ctx->num_custom_elems += 1;
}

bool update_ui(clay_ctx * ctx){
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
    for (int i = 0; i < 3; i++){
        if (pressed[i] != 0){
        TraceLog(LOG_DEBUG, "key: %s", kc_to_rep(pressed[i])); 
        }
    }
    if ( IsKeyDown(KEY_LEFT_CONTROL) && pressed[0] == KEY_ENTER && pressed[1] == 0){
        if (ctx->focused == NULL && ctx->num_custom_elems > 0) {
            ctx->focused = ctx->ced[0];
            return true;
        }
        uint foc_idx;
        for (int i = 0; i < ctx->num_custom_elems; i++){
            if (ctx->ced[i] == ctx->focused){
                foc_idx = i;
                break;
            }
        }
        if (foc_idx >= ctx->num_custom_elems -1) {
            ctx->focused = NULL;
            return true;
        }
        foc_idx++;
        ctx->focused = ctx->ced[foc_idx];
        return true;
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
                    bool focused = ctx.ced[i] == ctx.focused;
                    layout_textbox(&(ctx.ced[i]->textbox), &ctx.fonts[0], focused);
                    break;
                default:
                    break;
            }
        }
        }
    return Clay_EndLayout();
}
#endif
