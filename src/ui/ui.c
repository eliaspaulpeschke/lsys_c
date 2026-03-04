#include "custom.h"
#include "elements/apply_box.h"
#include "elements/drawbox.h"
#include "elements/lrules_box.h"
#include "elements/lstring_box.h"
#include "elements/turtle_move_box.h"
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

void add_box(clay_ctx * ctx, CustomElementData (*mk_elem)() ){
    if (ctx->num_custom_elems >= MAX_CUSTOM_ELEMS) return;
    CustomElementData * elem = malloc(sizeof(CustomElementData));
    if (!elem) return;
    *elem = mk_elem();
    if (elem->error) {
        free(elem);
        return;
    }
    ctx->ced[ctx->num_custom_elems] = elem;
    ctx->num_custom_elems += 1;
}

UpdateReturnValue update_main_panel(clay_ctx * ctx){
    if (IsMouseButtonReleased(0)){
        if (Clay_PointerOver(Clay_GetElementId(CLAY_STRING("add_rulesbox")))) {
                add_box(ctx, mk_rulesbox_elem);
                return UPDATE_INTERACT;
        }
        if (Clay_PointerOver(Clay_GetElementId(CLAY_STRING("add_turtlebox")))) {
                add_box(ctx, mk_turtlebox_elem);
                return UPDATE_INTERACT;
        }
        if (Clay_PointerOver(Clay_GetElementId(CLAY_STRING("add_applybox")))) {
                add_box(ctx, mk_applybox_elem);
                return UPDATE_INTERACT;
        }
        if (Clay_PointerOver(Clay_GetElementId(CLAY_STRING("add_lstringbox")))) {
                add_box(ctx, mk_lstringbox_elem);
                return UPDATE_INTERACT;
        }
        if (Clay_PointerOver(Clay_GetElementId(CLAY_STRING("add_drawbox")))) {
                add_box(ctx, mk_drawbox_elem);
                return UPDATE_INTERACT;
        }
    }
    return UPDATE_NONE;
}

static int GRAB_IDX = -1;

#define MOUSEGRAB(idx, expr) if (GRAB_IDX == -1 || GRAB_IDX == idx) { \
    res = expr; \
    if (res.grab_mouse == true) { \
        GRAB_IDX = idx; \
    } else { \
        GRAB_IDX = -1; \
    } \
    if (res.interacted) return res; \
}

UpdateReturnValue update_ui(clay_ctx * ctx){
    UpdateReturnValue res;
    MOUSEGRAB(1, update_module_connections())
    MOUSEGRAB(2, update_connection_status())
    MOUSEGRAB(3, update_main_panel(ctx))

    for (int i = 0; i < ctx->num_custom_elems; i++){
        switch (ctx->ced[i]->type) {
            case CUSTOM_ELEM_T_rulesbox:
                MOUSEGRAB(100+i, update_lrules_box(&(ctx->ced[i]->rulesbox)))
                break;
            case CUSTOM_ELEM_T_turtle_box:
                MOUSEGRAB(100+i,update_turtle_move_box(&(ctx->ced[i]->turtlebox)))
                break; 
            case CUSTOM_ELEM_T_apply_box:
                MOUSEGRAB(100+i,update_applybox(&(ctx->ced[i]->applybox)))
                break; 
            case CUSTOM_ELEM_T_lstring_box:
                MOUSEGRAB(100+i,update_lstring_box(&(ctx->ced[i]->lstringbox)))
                break; 
            case CUSTOM_ELEM_T_drawbox:
                MOUSEGRAB(100+i,update_drawbox(&(ctx->ced[i]->drawbox)))
                break; 
            default:
                break;
        }
    } 

//  KeyboardKey pressed[3] = {GetKeyPressed(), GetKeyPressed(), GetKeyPressed()};
    
//  if (!IsKeyDown(KEY_LEFT_CONTROL)) return false;
//  unsigned int switch_idx = 0;
//  bool shift = false;
//  if (pressed[0] == KEY_LEFT_SHIFT 
//      || pressed[0] == KEY_RIGHT_SHIFT) {
//      switch_idx = 1;
//      shift = true;
//  } shift = shift 
//           || IsKeyDown(KEY_LEFT_SHIFT) 
//           || IsKeyDown(KEY_RIGHT_SHIFT);

//  switch (pressed[switch_idx]){
//      case KEY_ENTER:
//          if (ctx->focus_index < ((int)ctx->num_custom_elems - 1)
//              && (!shift)){
//              ctx->focus_index += 1;
//          }else{
//              ctx->focus_index = -1;
//          }
//          return true;
//      default: 
//          return false;
//  }
     return UPDATE_NONE;
}

void mk_addbtn(char * label, char * id){
    CLAY(CLAY_SID(CLAYIFY(id)), { .layout = { .sizing = {CLAY_SIZING_FIXED(24), CLAY_SIZING_FIXED(24)}
                                             , .childAlignment = {CLAY_ALIGN_X_CENTER, CLAY_ALIGN_Y_CENTER} }
                                 , .backgroundColor = Clay_Hovered() ? COL_ACCENT : COL_TRANSPARENT }) {
        TEXT_STANDARD(label);
    };
}

Clay_RenderCommandArray mk_layout(clay_ctx ctx){
    Clay_SetCurrentContext(ctx.ctx);
    Clay_SetDebugModeEnabled(true);
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
                        mk_addbtn("R", "add_rulesbox");
                        mk_addbtn("T", "add_turtlebox");
                        mk_addbtn("A", "add_applybox");
                        mk_addbtn("L", "add_lstringbox");
                        mk_addbtn("D", "add_drawbox");
                    };
        for (int i = 0; i < ctx.num_custom_elems; i++){
            switch (ctx.ced[i]->type) {
                case CUSTOM_ELEM_T_rulesbox:
                    layout_lrules_box(ctx.ced[i]->rulesbox, ctx.fonts);
                    break;
                case CUSTOM_ELEM_T_turtle_box:
                    layout_turtle_move_box(ctx.ced[i]->turtlebox, ctx.fonts);
                    break;
                case CUSTOM_ELEM_T_apply_box:
                    layout_applybox(ctx.ced[i]->applybox, ctx.fonts);
                    break;
                case CUSTOM_ELEM_T_lstring_box:
                    layout_lstring_box(ctx.ced[i]->lstringbox, ctx.fonts);
                    break;
                case CUSTOM_ELEM_T_drawbox:
                    layout_drawbox(ctx.ced[i]->drawbox);
                    break;
                default:
                    break;
            }
        }
        }
    return Clay_EndLayout();
}
