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
#include "../clay_renderer_raylib.c"

typedef struct {
    Clay_Context * ctx;
    uint font_num;
    Font * fonts; 
    CustomElementData * ced;
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

    CustomElementData * ced = mk_textbox(8192, "testbox");

    Clay_SetMeasureTextFunction(Raylib_MeasureText, fonts); 
    return (clay_ctx){
          .ctx = ctx
        , .font_num = 1
        , .fonts = fonts
        , .ced = ced
    };
}

Clay_RenderCommandArray mk_layout(clay_ctx ctx){
    update_textbox(&(ctx.ced->textbox));
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
                                                  , .layoutDirection = CLAY_TOP_TO_BOTTOM }
                                      , .backgroundColor = COL_TRANSPARENT }) {

                           layout_textbox(&(ctx.ced->textbox), &ctx.fonts[0]);
        }
    return Clay_EndLayout();
}


#endif
