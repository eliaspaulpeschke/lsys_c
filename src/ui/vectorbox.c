#include "vectorbox.h"
#include "valuebox.h"
#include "raylib.h"
#include <string.h>
#include "../clay/clay.h"
#include "common.h"

Vector1_box mk_vector1box(float step, bool is_unsigned){
    ValueBox x = mk_valuebox(VALUEBOX_TYPE_float, false);
    if (x.error == true) return ERR_VECTOR_1_BOX;
    x.float_value.step = step;
    if (is_unsigned) {
        x.float_value.min = 0.0f;
    }
    Vector1_box vb = (Vector1_box) {
          .error = false
        , .x = x 
        , .value = 0.0f
    };
    return vb;
}

Vector2_box mk_vector2box(float step){
    ValueBox x = mk_valuebox(VALUEBOX_TYPE_float, false);
    if (x.error == true) return ERR_VECTOR_2_BOX;
    ValueBox y = mk_valuebox(VALUEBOX_TYPE_float, false);
    if (y.error == true) {
        free_valuebox(x);
        return ERR_VECTOR_2_BOX;
    }
    x.float_value.step = step;
    y.float_value.step = step;
    Vector2_box vb = (Vector2_box) {
          .error = false
        , .x = x 
        , .y = y 
        , .value = (Vector2){0.0f, 0.0f}
    };
    return vb;
}

Vector3_box mk_vector3box(float step){
    ValueBox x = mk_valuebox(VALUEBOX_TYPE_float, false);
    if (x.error == true) return ERR_VECTOR_3_BOX;
    ValueBox y = mk_valuebox(VALUEBOX_TYPE_float, false);
    if (y.error == true) {
        free_valuebox(x);
        return ERR_VECTOR_3_BOX;
    }
    ValueBox z = mk_valuebox(VALUEBOX_TYPE_float, false);
    if (z.error == true) {
        free_valuebox(x);
        free_valuebox(y);
        return ERR_VECTOR_3_BOX;
    }
    x.float_value.step = step;
    y.float_value.step = step;
    z.float_value.step = step;

    Vector3_box vb = (Vector3_box) {
          .error = false
        , .x = x 
        , .y = y 
        , .z = z
        , .value = (Vector3){0.0f, 0.0f, 0.0f}
    };
    return vb;
}

Vector4_box mk_vector4box(float step, bool col){
    ValueBox x = mk_valuebox(VALUEBOX_TYPE_float, false);
    if (x.error == true) return ERR_VECTOR_4_BOX;
    ValueBox y = mk_valuebox(VALUEBOX_TYPE_float, false);
    if (y.error == true) {
        free_valuebox(x);
        return ERR_VECTOR_4_BOX;
    }
    ValueBox z = mk_valuebox(VALUEBOX_TYPE_float, false);
    if (z.error == true) {
        free_valuebox(x);
        free_valuebox(y);
        return ERR_VECTOR_4_BOX;
    }
    ValueBox w = mk_valuebox(VALUEBOX_TYPE_float, false);
    if (w.error == true) {
        free_valuebox(x);
        free_valuebox(y);
        free_valuebox(z);
        return ERR_VECTOR_4_BOX;
    }
    x.float_value.step = step;
    y.float_value.step = step;
    z.float_value.step = step;
    w.float_value.step = step;
    if (col) {
        x.float_value.min = 0.0f;
        y.float_value.min = 0.0f;
        z.float_value.min = 0.0f;
        w.float_value.min = 0.0f;
        x.float_value.max = 255.0f;
        y.float_value.max = 255.0f;
        z.float_value.max = 255.0f;
        w.float_value.max = 255.0f;
    }
    Vector4_box vb = (Vector4_box) {
          .error = false
        , .x = x 
        , .y = y 
        , .z = z
        , .w = w
        , .value = (Vector4){0.0f, 0.0f, 0.0f, 0.0f}
    };
    return vb;
}

void free_vector1box(Vector1_box vb){
    free_valuebox(vb.x);
}
void free_vector2box(Vector2_box vb){
    free_valuebox(vb.x);
    free_valuebox(vb.y);
}

void free_vector3box(Vector3_box vb){
    free_valuebox(vb.x);
    free_valuebox(vb.y);
    free_valuebox(vb.z);
}

void free_vector4box(Vector4_box vb){
    free_valuebox(vb.x);
    free_valuebox(vb.y);
    free_valuebox(vb.z);
    free_valuebox(vb.w);
}

Clay_ElementDeclaration vector_box_conf = (Clay_ElementDeclaration){
            .layout = { .sizing = { .width = CLAY_SIZING_GROW(0)
                                  , .height = CLAY_SIZING_GROW(0) }
                      , .layoutDirection = CLAY_TOP_TO_BOTTOM
                      , .childGap = 4
                      , .padding = { 8, 8, 8, 8 }
                      , .childAlignment = { .x = CLAY_ALIGN_X_LEFT
                                          , .y = CLAY_ALIGN_Y_CENTER}
                      }
    };


void layout_vector1box(Vector1_box vb, Font * fonts, char * label){
    CLAY_AUTO_ID( vector_box_conf ) {
        add_label(label);
        layout_valuebox(vb.x, fonts, "x");
    };
}
void layout_vector2box(Vector2_box vb, Font * fonts, char * label){
    CLAY_AUTO_ID( vector_box_conf ) {
        add_label(label);
        layout_valuebox(vb.x, fonts, "x");
        layout_valuebox(vb.y, fonts, "y");
    };
}

void layout_vector3box(Vector3_box vb, Font * fonts, char * label){
    CLAY_AUTO_ID( vector_box_conf ) {
        add_label(label);
        layout_valuebox(vb.x, fonts, "x");
        layout_valuebox(vb.y, fonts, "y");
        layout_valuebox(vb.z, fonts, "z");
    };
}

void layout_vector4box(Vector4_box vb, Font * fonts, bool col, char * label){
    CLAY_AUTO_ID(vector_box_conf) {
        add_label(label);
        layout_valuebox(vb.x, fonts, col ? "r" : "x");
        layout_valuebox(vb.y, fonts, col ? "g" : "y");
        layout_valuebox(vb.z, fonts, col ? "b" : "z");
        layout_valuebox(vb.w, fonts, col ? "a" : "w");
    };
}

UpdateReturnValue update_vector_component(ValueBox * vb, float * comp){
  UpdateReturnValue res = update_valuebox(vb);
  if (vb->changed) {
     vb->changed = false;
     *comp = vb->float_value.value;
  }
  return res;
}
UpdateReturnValue update_vector1box(Vector1_box *vb){
    if (update_vector_component(&(vb->x), &(vb->value)).interacted) 
        return (UpdateReturnValue) {.interacted = true, .grab_mouse = false};
    return (UpdateReturnValue) {.interacted = false, .grab_mouse = false};
}


UpdateReturnValue update_vector2box(Vector2_box *vb){
    if (update_vector_component(&(vb->x), &(vb->value.x)).interacted) 
        return (UpdateReturnValue) {.interacted = true, .grab_mouse = false};
    if (update_vector_component(&(vb->y), &(vb->value.y)).interacted) 
        return (UpdateReturnValue) {.interacted = true, .grab_mouse = false};
    return (UpdateReturnValue) {.interacted = false, .grab_mouse = false};
}

UpdateReturnValue update_vector3box(Vector3_box *vb){
    if (update_vector_component(&(vb->x), &(vb->value.x)).interacted) 
        return (UpdateReturnValue) {.interacted = true, .grab_mouse = false};
    if (update_vector_component(&(vb->y), &(vb->value.y)).interacted) 
        return (UpdateReturnValue) {.interacted = true, .grab_mouse = false};
    if (update_vector_component(&(vb->z), &(vb->value.z)).interacted) 
        return (UpdateReturnValue) {.interacted = true, .grab_mouse = false};
    return (UpdateReturnValue) {.interacted = false, .grab_mouse = false};
}

UpdateReturnValue update_vector4box(Vector4_box *vb){
    if (update_vector_component(&(vb->x), &(vb->value.x)).interacted) 
        return (UpdateReturnValue) {.interacted = true, .grab_mouse = false};
    if (update_vector_component(&(vb->y), &(vb->value.y)).interacted) 
        return (UpdateReturnValue) {.interacted = true, .grab_mouse = false};
    if (update_vector_component(&(vb->z), &(vb->value.z)).interacted) 
        return (UpdateReturnValue) {.interacted = true, .grab_mouse = false};
    if (update_vector_component(&(vb->w), &(vb->value.w)).interacted) 
        return (UpdateReturnValue) {.interacted = true, .grab_mouse = false};
    return (UpdateReturnValue) {.interacted = false, .grab_mouse = false};
}
