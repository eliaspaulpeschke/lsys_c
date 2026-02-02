#include "raylib.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define CLAY_IMPLEMENTATION
#include "lib/clay.h"
#include "lib/clay_renderer_raylib.c"
#include "lib/ui/textbox.c"
typedef struct {
    Clay_Context * ctx;
    uint font_num;
    Font * fonts; 
    textbox * tb;
} clay_ctx;
const Clay_Color CLAY_LIGHT = (Clay_Color) {224, 215, 210, 255};
const Clay_Color CLAY_RED = (Clay_Color) {168, 66, 28, 255};
const Clay_Color CLAY_ORANGE = (Clay_Color) {225, 138, 50, 255};


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

    Clay_SetMeasureTextFunction(Raylib_MeasureText, fonts); 
    return (clay_ctx){
          .ctx = ctx
        , .font_num = 1
        , .fonts = fonts
        , .tb = tb
    };
}

// Layout config is just a struct that can be declared statically, or inline
Clay_ElementDeclaration sidebarItemConfig = (Clay_ElementDeclaration) {
    .layout = {
        .sizing = { .width = CLAY_SIZING_GROW(0), .height = CLAY_SIZING_FIXED(50) }
    },
    .backgroundColor = CLAY_ORANGE
};

// Re-useable components are just normal functions
void SidebarItemComponent(Clay_ElementId id) {
    CLAY(id, sidebarItemConfig) {
        // children go here...
    }
}

Clay_RenderCommandArray mk_layout(clay_ctx ctx){
    update_textbox(ctx.tb);
    Clay_SetCurrentContext(ctx.ctx);
    Clay_SetDebugModeEnabled(true);
    Vector2 mouse = GetMousePosition();
    Clay_SetPointerState( (Clay_Vector2) {mouse.x, mouse.y}
                        , IsMouseButtonDown(0)
                        );
    Vector2 scroll = GetMouseWheelMoveV();
    Clay_UpdateScrollContainers( true
                               , (Clay_Vector2) {scroll.x, scroll.y}
                               , GetFrameTime()
                               );
    Clay_SetLayoutDimensions((Clay_Dimensions){.width = GetScreenWidth(),
                                               .height = GetScreenHeight()});
    Clay_BeginLayout();
    
    CLAY(CLAY_ID("OuterContainer")
        , { .layout = { .sizing = {CLAY_SIZING_GROW(0), CLAY_SIZING_GROW(0)}
                      , .padding = CLAY_PADDING_ALL(16)
                      , .childGap = 16 }
        , .backgroundColor = {0,0,0,0} }) {
          
          CLAY(CLAY_ID("SideBar")
               , { .layout = { .layoutDirection = CLAY_TOP_TO_BOTTOM
                             , .sizing = { .width = CLAY_SIZING_FIXED(300)
                             , .height = CLAY_SIZING_GROW(0) }
                             , .padding = CLAY_PADDING_ALL(16)
                             , .childGap = 16 }
                 , .backgroundColor = CLAY_LIGHT }) {
            
                   CLAY(CLAY_ID("ProfilePictureOuter")
                       , { .layout = { .sizing = { .width = CLAY_SIZING_GROW(0) }
                                     , .padding = CLAY_PADDING_ALL(16)
                                     , .childGap = 16
                                     , .childAlignment = { .y = CLAY_ALIGN_Y_CENTER } }
                        , .backgroundColor = CLAY_RED }) {
                        
                          CLAY(CLAY_ID("ProfilePicture"), {.layout = { .sizing = { .width = CLAY_SIZING_FIXED(60), .height = CLAY_SIZING_FIXED(60) }} }) {}
                          CLAY_TEXT( CLAY_STRING("Clay - UI Library")
                                   , CLAY_TEXT_CONFIG({ .fontSize = 16, .fontId = 0, .textColor = {255, 255, 255, 255} }));
                      }

                   for (int i = 0; i < 5; i++) {
                       SidebarItemComponent(CLAY_IDI("hi", i));
                   }

                   CLAY(CLAY_ID("MainContent"), { .layout = { .sizing = { .width = CLAY_SIZING_FIXED(400), .height = CLAY_SIZING_FIXED(400)} }, .backgroundColor = CLAY_LIGHT}) {
                       layout_textbox(ctx.tb);
                   }
            }
        }
    return Clay_EndLayout();
}


