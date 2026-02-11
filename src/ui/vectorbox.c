#include "vectorbox.h"
#include "inputbox.h"
#include "raylib.h"
#include <string.h>
#include "../clay/clay.h"
#include "common.h"

Vector2_box mk_vector2box(){
    Inputbox x = mk_inputbox(32);
    if (x.error == true) return ERR_VECTOR_2_BOX;
    Inputbox y = mk_inputbox(32);
    if (y.error == true) {
        free_inputbox(x);
        return ERR_VECTOR_2_BOX;
    }
    Vector2_box vb = (Vector2_box) {
          .error = false
        , .x = x 
        , .y = y 
        , .value = (Vector2){0.0f, 0.0f}
    };
    set_inputbox_text(vb.x, "0.0");
    set_inputbox_text(vb.y, "0.0");
    return vb;
}

Vector3_box mk_vector3box(){
    Inputbox x = mk_inputbox(32);
    if (x.error == true) return ERR_VECTOR_3_BOX;
    Inputbox y = mk_inputbox(32);
    if (y.error == true) {
        free_inputbox(x);
        return ERR_VECTOR_3_BOX;
    }
    Inputbox z = mk_inputbox(32);
    if (z.error == true) {
        free_inputbox(x);
        free_inputbox(y);
        return ERR_VECTOR_3_BOX;
    }
    Vector3_box vb = (Vector3_box) {
          .error = false
        , .x = x 
        , .y = y 
        , .z = z
        , .value = (Vector3){0.0f, 0.0f, 0.0f}
    };
    return vb;
    set_inputbox_text(vb.x, "0.0");
    set_inputbox_text(vb.y, "0.0");
    set_inputbox_text(vb.z, "0.0");
}

Vector4_box mk_vector4box(){
    Inputbox x = mk_inputbox(32);
    if (x.error == true) return ERR_VECTOR_4_BOX;
    Inputbox y = mk_inputbox(32);
    if (y.error == true) {
        free_inputbox(x);
        return ERR_VECTOR_4_BOX;
    }
    Inputbox z = mk_inputbox(32);
    if (z.error == true) {
        free_inputbox(x);
        free_inputbox(y);
        return ERR_VECTOR_4_BOX;
    }
    Inputbox w = mk_inputbox(32);
    if (w.error == true) {
        free_inputbox(x);
        free_inputbox(y);
        free_inputbox(z);
        return ERR_VECTOR_4_BOX;
    }
    Vector4_box vb = (Vector4_box) {
          .error = false
        , .x = x 
        , .y = y 
        , .z = z
        , .w = w
        , .value = (Vector4){0.0f, 0.0f, 0.0f, 0.0f}
    };
    set_inputbox_text(vb.x, "0.0");
    set_inputbox_text(vb.y, "0.0");
    set_inputbox_text(vb.z, "0.0");
    set_inputbox_text(vb.w, "0.0");

    return vb;
}


void free_vector2box(Vector2_box vb){
    free_inputbox(vb.x);
    free_inputbox(vb.y);
}

void free_vector3box(Vector3_box vb){
    free_inputbox(vb.x);
    free_inputbox(vb.y);
    free_inputbox(vb.z);
}

void free_vector4box(Vector4_box vb){
    free_inputbox(vb.x);
    free_inputbox(vb.y);
    free_inputbox(vb.z);
    free_inputbox(vb.w);
}

Clay_ElementDeclaration vector_box_conf = (Clay_ElementDeclaration){
            .layout = { .sizing = { .width = CLAY_SIZING_GROW(0)
                                  , .height = CLAY_SIZING_GROW(0) }
                      , .layoutDirection = CLAY_TOP_TO_BOTTOM
                      , .childGap = 8 
                      , .childAlignment = { .x = CLAY_ALIGN_X_LEFT
                                          , .y = CLAY_ALIGN_Y_CENTER}
                      }
    };



void layout_vector2box(Vector2_box vb, Font * fonts, char * label){
    CLAY_AUTO_ID( vector_box_conf ) {
        add_label(label);
        layout_inputbox(vb.x, fonts, false, "x: ");
        layout_inputbox(vb.y, fonts, false, "y: ");
    };
}

void layout_vector3box(Vector3_box vb, Font * fonts, char * label){
    CLAY_AUTO_ID( vector_box_conf ) {
        add_label(label);
        layout_inputbox(vb.x, fonts, false, "x: ");
        layout_inputbox(vb.y, fonts, false, "y: ");
        layout_inputbox(vb.z, fonts, false, "z: ");
    };
}

void layout_vector4box(Vector4_box vb, Font * fonts, char * label){
    CLAY_AUTO_ID(vector_box_conf) {
        add_label(label);
        layout_inputbox(vb.x, fonts, false, "x: ");
        layout_inputbox(vb.y, fonts, false, "y: ");
        layout_inputbox(vb.z, fonts, false, "z: ");
        layout_inputbox(vb.w, fonts, false, "u: ");
    };
}

bool update_vector_component(Inputbox * ipb, float * comp){
  bool res = update_inputbox(ipb);
  if (res) {
     if (ipb->changed) {
         ipb->changed = false;
         *comp = strtof(ipb->text, NULL);
     }
  }
  return res;
}

bool update_vector2box(Vector2_box *vb){
    if (update_vector_component(&(vb->x), &(vb->value.x))) return true;
    if (update_vector_component(&(vb->y), &(vb->value.y))) return true;
    return false;
}

bool update_vector3box(Vector3_box *vb){
    if (update_vector_component(&(vb->x), &(vb->value.x))) return true;
    if (update_vector_component(&(vb->y), &(vb->value.y))) return true;
    if (update_vector_component(&(vb->z), &(vb->value.z))) return true;
    return false;
}

bool update_vector4box(Vector4_box *vb){
    if (update_vector_component(&(vb->x), &(vb->value.x))) return true;
    if (update_vector_component(&(vb->y), &(vb->value.y))) return true;
    if (update_vector_component(&(vb->z), &(vb->value.z))) return true;
    if (update_vector_component(&(vb->w), &(vb->value.w))) return true;
    return false;
}
