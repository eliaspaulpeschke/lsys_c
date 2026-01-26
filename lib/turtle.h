#include "string.h"
#include <raylib.h>
#include <stdlib.h>
#include <raymath.h>

#define TURTLE_H

typedef struct Turtle {
    Vector2 pos;
    Vector2 heading;
    float length;
    float rads;
    struct Turtle * prev;
} Turtle;

void rotateTurtle(Turtle * turtle, bool ccw){
    if (ccw) {
        turtle->heading = Vector2Rotate(turtle->heading, turtle->rads);
    } else { 
        turtle->heading = Vector2Rotate(turtle->heading, -turtle->rads);   }
}

void moveTurtle(Turtle * turtle){
    Vector2 new = Vector2Add(turtle->pos, turtle->heading);
    DrawLineV(turtle->pos, new, WHITE);
    turtle->pos = new;
}

void moveTurtleCol(Turtle * turtle, Color col){
    Vector2 new = Vector2Add(turtle->pos, turtle->heading);
    DrawLineV(turtle->pos, new, col);
    turtle->pos = new;
}

Turtle * pushTurtle(Turtle * turtle){
    Turtle * t = malloc(sizeof(Turtle));
    memcpy(t, turtle, sizeof(Turtle));
    t->prev = turtle;
    return t;
}

Turtle * popTurtle(Turtle * turtle){
    return turtle->prev;
}
