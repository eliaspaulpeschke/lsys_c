#include "move_container.h"
#include "common.h"
#include "raylib.h"
#include "raymath.h"

static unsigned int NUM_MOVE_CONTAINERS = 0;

Move_container mk_move_container( void (*move_hook)
                                    ( Vector2 delta
                                    , void * user_data)
                                , void (*resize_hook)
                                    ( Vector2 old_size
                                    , Vector2 new_size
                                    , void * user_data)
                                ){
    Move_container cont = (Move_container) {
          .pos = (Vector2) {0.0, 0.0}
        , .size = (Vector2) {0.0, 0.0}
        , .clay_id_idx = NUM_MOVE_CONTAINERS 
    };
    NUM_MOVE_CONTAINERS += 1;
    return cont;
}

Clay_ElementDeclaration move_cont_clay_decl(Move_container cont, int padding, bool resizable){
    Clay_Sizing fix = (Clay_Sizing){CLAY_SIZING_FIXED(cont.size.x),CLAY_SIZING_FIXED(cont.size.y)};
    Clay_Sizing fit = (Clay_Sizing){CLAY_SIZING_FIT(0),CLAY_SIZING_FIT(0)};
    Clay_ElementDeclaration decl = (Clay_ElementDeclaration) {
      .layout = { .sizing = resizable ? fix : fit 
                , .padding = CLAY_PADDING_ALL(padding) 
                , .childGap = 8}
    , .floating = { .attachTo = CLAY_ATTACH_TO_ROOT, .offset = {.x = cont.pos.x, .y = cont.pos.y}
                  , .zIndex = MOVE_CONTAINER_Z_INDEX }
    , .border = {.color = COL_DARK, .width = {0,0,0,0,1}}
    , .backgroundColor = COL_LIGHT };
    return decl;
}

void layout_move_container_sizer(Move_container cont){
    CLAY(CLAY_IDI("move-container-sizer", cont.clay_id_idx), {
        .layout = { .sizing = { CLAY_SIZING_FIXED(32),CLAY_SIZING_FIXED(32) } 
                  , .childAlignment = {.x = CLAY_ALIGN_X_CENTER , .y = CLAY_ALIGN_Y_CENTER} }
      , .floating = { .attachTo = CLAY_ATTACH_TO_PARENT, .offset = {.x = cont.size.x - 24, .y = cont.size.y - 24}
                  , .zIndex = MOVE_CONTAINER_Z_INDEX}
      , .backgroundColor = COL_TRANSPARENT }
        ){
        CLAY_AUTO_ID({
                .layout = {.sizing = { CLAY_SIZING_FIXED(8), CLAY_SIZING_FIXED(8) } }
                , .backgroundColor = COL_DARK
                , .cornerRadius = CLAY_CORNER_RADIUS(4)
                });
     }
    ;
}

bool update_move_container(Move_container * cont, bool resizable, void * user_data){
    bool ptr =Clay_PointerOver(Clay_GetElementIdWithIndex(CLAY_STRING("move-container"), cont->clay_id_idx));
    bool sizer=Clay_PointerOver(Clay_GetElementIdWithIndex(CLAY_STRING("move-container-sizer"), cont->clay_id_idx));
 
if ((ptr || sizer) && IsMouseButtonDown(0)){
        Vector2 mouse = GetMouseDelta();
        if (sizer && resizable) {
            Vector2 old = cont->size;
            cont->size = Vector2Add(mouse, cont->size);
            if (cont->resize_hook != NULL){
                cont->resize_hook(old, cont->size, user_data);
            }
        } else {
            cont->pos = Vector2Add(mouse, cont->pos);
            if (cont->move_hook != NULL){
                cont->move_hook(mouse,user_data);
            }
        }
        return true;
    }
return false;
}

Clay_ElementId move_cont_clay_id(Move_container cont){
    return CLAY_IDI("move-container", cont.clay_id_idx);
}
