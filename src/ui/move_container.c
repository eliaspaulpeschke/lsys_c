#include "move_container.h"
#include "common.h"
#include "raymath.h"

static unsigned int NUM_MOVE_CONTAINERS = 0;

Move_container mk_move_container(){
    Move_container cont = (Move_container) {
          .pos = (Vector2) {0.0, 0.0}
        , .size = (Vector2) {0.0, 0.0}
        , .clay_id_idx = NUM_MOVE_CONTAINERS + NAMESPACE_MOVE_CONT
    };
    NUM_MOVE_CONTAINERS += 1;
    return cont;
}

Clay_ElementDeclaration move_cont_clay_decl(Move_container cont){
    Clay_ElementDeclaration decl = (Clay_ElementDeclaration) {
      .layout = { .sizing = { CLAY_SIZING_FIXED(cont.size.x),CLAY_SIZING_FIXED(cont.size.x) }
                , .padding = CLAY_PADDING_ALL(8) }
    , .floating = { .attachTo = CLAY_ATTACH_TO_ROOT, .offset = {.x = cont.pos.x, .y = cont.pos.y}
                  , .zIndex = MOVE_CONTAINER_Z_INDEX }
    , .backgroundColor = COL_LIGHT };
    return decl;
}

void layout_move_container_sizer(Move_container cont){
    CLAY(CLAY_IDI("move-container-sizer", cont.clay_id_idx), {
        .layout = { .sizing = { CLAY_SIZING_FIXED(8),CLAY_SIZING_FIXED(8) }
                , .padding = CLAY_PADDING_ALL(8) }
                , .cornerRadius = CLAY_CORNER_RADIUS(4)
    , .floating = { .attachTo = CLAY_ATTACH_TO_PARENT, .offset = {.x = cont.size.x - 24, .y = cont.size.y - 24}
                  , .zIndex = MOVE_CONTAINER_Z_INDEX}
    , .backgroundColor = COL_DARK }
            );
}

bool update_move_container(Move_container * cont){
    bool ptr =Clay_PointerOver(Clay_GetElementIdWithIndex(CLAY_STRING("move-container"), cont->clay_id_idx));
    bool sizer=Clay_PointerOver(Clay_GetElementIdWithIndex(CLAY_STRING("move-container-sizer"), cont->clay_id_idx));
 
if (ptr && IsMouseButtonDown(0)){
        Vector2 mouse = GetMouseDelta();
        if (sizer) {
            cont->size = Vector2Add(mouse, cont->size);
        } else {
            cont->pos = Vector2Add(mouse, cont->pos);
        }
        return true;
    }
return false;
}

Clay_ElementId move_cont_clay_id(Move_container cont){
    return CLAY_IDI("move-container", cont.clay_id_idx);
}
