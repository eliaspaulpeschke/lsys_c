#ifndef TURTLE_H
#define TURTLE_H
#include <raylib.h>
#include "../lsystem/lsystem.h"

typedef struct Turtle {
    struct Turtle * prev;
    float length;
    float rads;
    Vector2 pos;
    Vector2 heading;
    Color color;
    Vector2 positionQueue[4];
} Turtle;

void rotate_turtle(Turtle * turtle, bool ccw);
void move_turtle(Turtle * turtle);
void move_turtle_col(Turtle * turtle, Color col);
void turtle_set_color(Turtle * turtle, Color col);
Turtle * push_turtle(Turtle * turtle);
Turtle * pop_turtle(Turtle * turtle);
Turtle mk_base_turtle();
void turtle_draw(LString * text, unsigned int len, Turtle turtle);
void standard_turtle_draw(LString * text);

#endif
