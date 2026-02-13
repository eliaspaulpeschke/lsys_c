#ifndef VECTORBOX_H
#define VECTORBOX_H

#include "inputbox.h"
#include "raylib.h"

typedef struct {
    bool error;
    Inputbox x;
    float value;
} Vector1_box;

typedef struct {
    bool error;
    Inputbox x;
    Inputbox y;
    Vector2 value;
} Vector2_box;

typedef struct {
    bool error;
    Inputbox x;
    Inputbox y;
    Inputbox z;
    Vector3 value;
} Vector3_box;

typedef struct {
    bool error;
    Inputbox x;
    Inputbox y;
    Inputbox z;
    Inputbox w;
    Vector4 value;
} Vector4_box;

#define ERR_VECTOR_1_BOX (Vector1_box){true, ERR_INPUTBOX, 0 }
#
#define ERR_VECTOR_2_BOX (Vector2_box){true, ERR_INPUTBOX, ERR_INPUTBOX, {0,0} }
#define ERR_VECTOR_3_BOX (Vector3_box){true, ERR_INPUTBOX,ERR_INPUTBOX, ERR_INPUTBOX, {0,0,0} }
#define ERR_VECTOR_4_BOX (Vector4_box){true, ERR_INPUTBOX,ERR_INPUTBOX,ERR_INPUTBOX, ERR_INPUTBOX, {0,0,0,0} }

Vector1_box mk_vector1box();
Vector2_box mk_vector2box();
Vector3_box mk_vector3box();
Vector4_box mk_vector4box();

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
