

#include "raylib.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define CLAY_IMPLEMENTATION
#include "lib/clay.h"
#include "lib/clay_renderer_raylib.c"

typedef struct {
    char * bufA;
    char * bufB;
    uint lenA;
    uint lenB;
    uint posA;
    uint posB; //used backwards, start at lenB
    uint max_len;
} textbox;

typedef struct {
    Clay_Context * ctx;
    uint font_num;
    Font * fonts; 
    textbox * tb;
} clay_ctx;

bool realloc_bufA(textbox * tb){
  TraceLog(LOG_DEBUG, "realloc a");
  if ((tb->lenA << 1) + tb->lenB >= tb->max_len) return false;
  char * temp = realloc(tb->bufA, tb->lenA << 1);
  bool success = false;
  if (temp != NULL) {
      memset(tb->bufA + tb->lenA, '\0', tb->lenA);
      tb->bufA = temp;
      tb->lenA <<= 1;
      success = true;
  } 
  return success;
} 

bool realloc_bufB(textbox * tb){
  TraceLog(LOG_DEBUG, "realloc b");
  if ((tb->lenB << 1) + tb->lenA >= tb->max_len) return false;
  char * temp = malloc(tb->lenB << 1);
  bool success = false;
  if (temp != NULL) {
     memset(temp + tb->lenB, '\0', tb->lenB);
     memcpy(temp + tb->lenB, tb->bufB, tb->lenB);
     free(tb->bufB);
     tb->bufB = temp;
     tb->posB = tb->lenB;
     tb->lenB <<= 1;
     success = true;
  }
  return success;
}

void print_tb(textbox * tb){
    uint lb = tb->lenB - tb->posB;
    char * testA = malloc(tb->posA + 1);
    char * testB = malloc(lb);
    memcpy(testA, tb->bufA, tb->posA);
    memcpy(testB, tb->bufB + tb->posB + 1, lb - 1);
    testA[tb->posA] = '\0';
    testB[lb - 1] = '\0';
    TraceLog(LOG_DEBUG, "A: %s", testA); 
    TraceLog(LOG_DEBUG, "B: %s", testB); 
    free(testA);
    free(testB);
}

void update_textbox(textbox * tb){
    char chr = GetCharPressed();
    KeyboardKey key = GetKeyPressed();
    if (chr != 0){
        if (tb->posA >= tb->lenA) if (!realloc_bufA(tb)) return; 
        tb->bufA[tb->posA] = chr;
        tb->posA += 1;
        TraceLog(LOG_DEBUG,"charring %d", tb->posA); 
        print_tb(tb);
    } else if (key != KEY_NULL) {
        TraceLog(LOG_DEBUG,"keying"); 
        switch (key) {
            case KEY_BACKSPACE:
                if (tb->posA >= 0){
                    TraceLog(LOG_DEBUG,"backspace"); 
                    if (tb->posA > 0) tb->posA -= 1;
                    tb->bufA[tb->posA] = '\0';
                    // shrink buffer?
                }
                break;
            case KEY_LEFT:
                if (tb->posA > 0){
                    if (tb->posB == 0) if (!realloc_bufB(tb)) return;
                    tb->posA -= 1;
                    tb->bufB[tb->posB] = tb->bufA[tb->posA];
                    tb->bufA[tb->posA] = '\0';
                    tb->posB -= 1;
                    TraceLog(LOG_DEBUG,"left %d", tb->posB);
                }
                break;
            case KEY_RIGHT:
                if (tb->posB < (tb->lenB - 1)) {
                    if (tb->posA >= tb->lenA) if (!realloc_bufA(tb)) return; 
                    TraceLog(LOG_DEBUG,"right"); 
                    tb->bufA[tb->posA] = tb->bufB[tb->posB + 1];
                    tb->posB++;
                    tb->posA++;
                }
                break;
            default:
                TraceLog(LOG_DEBUG,"your car"); 
                return;
            }
        }
}

void layout_textbox(textbox * tb){
  char * text = malloc(tb->posA + (tb->lenB - tb->posB));
  memcpy(text,tb->bufA,tb->posA);
  memcpy(text+tb->posA,tb->bufB + tb->posB + 1, tb->lenB - tb->posB - 1);
  Clay_String str = (Clay_String){.isStaticallyAllocated = false, .length = tb->posA + (tb->lenB - tb->posB) - 1, .chars = text};
  CLAY_TEXT( str 
           , CLAY_TEXT_CONFIG({ .fontSize = 16, .fontId = 0, .textColor = {255, 255, 255, 255} }));
}

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

    textbox * tb = malloc(sizeof(textbox));
    *tb = (textbox){ .bufA = malloc(1024)
                          , .bufB = malloc(1024)
                          , .lenA = 1024
                          , .lenB = 1024
                          , .posA = 0
                          , .posB = 1023
                          , .max_len = 2048
                          };

    memset(tb->bufA, '\0', tb->lenA);
    memset(tb->bufB, '\0', tb->lenB);

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

                   CLAY(CLAY_ID("MainContent"), { .layout = { .sizing = { .width = CLAY_SIZING_GROW(0), .height = CLAY_SIZING_GROW(0) } }, .backgroundColor = CLAY_LIGHT}) {
                       layout_textbox(ctx.tb);
                   }
            }
        }
    return Clay_EndLayout();
}


