#ifndef MOVE_CONTAINER_H
#define MOVE_CONTAINER_H

#define MOVE_CONTAINER_Z_INDEX 0
#include "../clay/clay.h"
#include "raylib.h"

typedef struct {
    Vector2 pos;
    Vector2 size;
    unsigned int clay_id_idx;
} Move_container;

Clay_ElementDeclaration move_cont_clay_decl(Move_container cont, int padding, bool resizable);

Clay_ElementId move_cont_clay_id(Move_container cont);
bool update_move_container(Move_container * cont, bool resizable);
void layout_move_container_sizer(Move_container cont);
Move_container mk_move_container();
#endif
