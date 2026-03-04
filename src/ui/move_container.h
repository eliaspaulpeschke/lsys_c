#ifndef MOVE_CONTAINER_H
#define MOVE_CONTAINER_H

#include "common.h"
#define MOVE_CONTAINER_Z_INDEX 0
#include "../clay/clay.h"
#include "raylib.h"

typedef enum {
    MOVE_CONTAINER_STATE_NONE
  , MOVE_CONTAINER_STATE_RESIZE
  , MOVE_CONTAINER_STATE_MOVE
} MoveContainerState;

typedef struct {
    bool mouse_grabbed;
    bool minimized;
    MoveContainerState state;
    Vector2 pos;
    Vector2 size;
    unsigned int clay_id_idx;
    void (*move_hook)(Vector2 delta, void * user_data);
    void (*resize_hook)(Vector2 old_size, Vector2 new_size, void * user_data);
} Move_container;

Clay_ElementDeclaration move_cont_clay_decl(Move_container cont);
Clay_ElementDeclaration move_cont_clay_decl_inner(Move_container cont, int padding, bool resizable);


Clay_ElementId move_cont_clay_id(Move_container cont);
UpdateReturnValue update_move_container(Move_container * cont, bool resizable, void * user_data);
void layout_move_container_sizer(Move_container cont);
void layout_move_container_bar(Move_container cont);
Move_container mk_move_container( void (*move_hook)
                                    ( Vector2 delta
                                    , void * user_data)
                                , void (*resize_hook)
                                    ( Vector2 old_size
                                    , Vector2 new_size
                                    , void * user_data)
                                );

#define LAYOUT_MOVE_CONTAINER(cont, maximized_content, minimized_content, padding, resizable) \
\
CLAY(move_cont_clay_id(cont), move_cont_clay_decl(cont)){\
    layout_move_container_bar(cont); \
    CLAY_AUTO_ID(move_cont_clay_decl_inner(cont, padding, cont.minimized ? false : resizable)) {\
        if (cont.minimized) {\
            minimized_content;\
        } else {\
            maximized_content;\
        } \
        if (resizable && !cont.minimized) {\
            layout_move_container_sizer(cont); \
        } \
    }\
} 
#endif
