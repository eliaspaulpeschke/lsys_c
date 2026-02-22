#ifndef VECTORBOX_H
#define VECTORBOX_H

#include "raylib.h"
#include "valuebox.h"

typedef struct {
    bool error;
    ValueBox x;
    float value;
    float step;
} Vector1_box;

typedef struct {
    bool error;
    ValueBox x;
    ValueBox y;
    Vector2 value;
    float step;
} Vector2_box;

typedef struct {
    bool error;
    ValueBox x;
    ValueBox y;
    ValueBox z;
    Vector3 value;
    float step;
} Vector3_box;

typedef struct {
    bool error;
    ValueBox x;
    ValueBox y;
    ValueBox z;
    ValueBox w;
    Vector4 value;
    float step;
} Vector4_box;

#define ERR_VECTOR_1_BOX (Vector1_box){true, ERR_VALUEBOX, 0 }
#define ERR_VECTOR_2_BOX (Vector2_box){true, ERR_VALUEBOX, ERR_VALUEBOX, {0,0} }
#define ERR_VECTOR_3_BOX (Vector3_box){true, ERR_VALUEBOX, ERR_VALUEBOX, ERR_VALUEBOX, {0,0,0} }
#define ERR_VECTOR_4_BOX (Vector4_box){true, ERR_VALUEBOX, ERR_VALUEBOX, ERR_VALUEBOX, ERR_VALUEBOX, {0,0,0,0} }

Vector1_box mk_vector1box(float step, bool is_unsigned);
Vector2_box mk_vector2box(float step);
Vector3_box mk_vector3box(float step);
Vector4_box mk_vector4box(float step, bool col);

void free_vector1box(Vector1_box vb);
void free_vector2box(Vector2_box vb);
void free_vector3box(Vector3_box vb);
void free_vector4box(Vector4_box vb);

void layout_vector1box(Vector1_box vb, Font * fonts, char * label);
void layout_vector2box(Vector2_box vb, Font * fonts, char * label);
void layout_vector3box(Vector3_box vb, Font * fonts, char * label);
void layout_vector4box(Vector4_box vb, Font * fonts, bool col, char * label);

bool update_vector1box(Vector1_box * vb);
bool update_vector2box(Vector2_box * vb);
bool update_vector3box(Vector3_box * vb);
bool update_vector4box(Vector4_box * vb);
#endif
