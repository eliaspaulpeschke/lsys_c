#include "drawbox.h"
#include "../../clay/clay.h"
#include "../../turtle/turtle.h"
#include "../drawhook.h"
#include "../common.h"
#include <raylib.h>
#include <stdlib.h>


DrawBox mk_drawbox(){
    Module * lstring_in = mk_module(MODULE_INPUT, MODULE_DATA_TYPE_lstring);
    if (lstring_in->type == MODULE_NONE) return ERR_DRAWBOX;
    Module * turtle_in = mk_module(MODULE_INPUT, MODULE_DATA_TYPE_turtle);
    if (turtle_in->type == MODULE_NONE) {
        free(lstring_in);
        return ERR_DRAWBOX;
    }
    DrawBox db = (DrawBox){ .error = false
                          , .update_handle = -1
                          , .lstring_in = lstring_in
                          , .turtle_in = turtle_in
                          , .cont = mk_move_container(NULL, NULL)
                          };
    return db;
}

void free_drawbox(DrawBox db){
    free(db.turtle_in);
    free(db.lstring_in);
}

void draw_draw_box(void * user_data){
    DrawBox * db = (DrawBox*) user_data;
    Turtle turt = db->turtle_in->input.connection->output.turtle;
    LString lstr = db->lstring_in->input.connection->output.lstring;
    turtle_draw(&lstr, lstr.length, turt);
}

void always_update_drawbox(void * user_data){
    DrawBox * db = (DrawBox *)user_data;
    if (  db->lstring_in->input.connection != NULL
       && db->turtle_in->input.connection != NULL
       && db->lstring_in->input.connection->output.valid 
       && db->turtle_in->input.connection->output.valid ){
         add_draw_hook(draw_draw_box, db);
    }
}

UpdateReturnValue update_drawbox(DrawBox *db){
    if (db->update_handle < 0) {
        db->update_handle = add_update_hook(always_update_drawbox, db);
    }
    if (update_module(db->lstring_in).interacted) return UPDATE_INTERACT;
    if (update_module(db->turtle_in).interacted) return UPDATE_INTERACT;
    if (update_move_container(&db->cont, false, NULL).interacted) return UPDATE_INTERACT;
    return UPDATE_NONE;
}

void layout_drawbox(DrawBox db){
    CLAY( move_cont_clay_id(db.cont)
        , move_cont_clay_decl(db.cont, 8, false) ){
        CLAY_AUTO_ID({.layout = {SIZE_GROW_XY(0), LAYOUT_TB}}){
            layout_module(*db.lstring_in);
            layout_module(*db.turtle_in);
        };
        CLAY_AUTO_ID({.layout = {SIZE_FIX_XY(80, 40)}}){};
    };
}
