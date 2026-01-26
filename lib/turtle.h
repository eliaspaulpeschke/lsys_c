#include <raylib.h>
#include <raymath.h>

#define TURTLE_H

typedef struct Turtle {
    Vector2 pos;
    Vector2 heading;
    float length;
    float rads;
} Turtle;

Turtle rotateTurtle(Turtle turtle, bool ccw){
    if (ccw) {
        turtle.heading = Vector2Rotate(turtle.heading, turtle.rads);
    } else { 
        turtle.heading = Vector2Rotate(turtle.heading, -turtle.rads);   }
    return turtle;
}

Turtle moveTurtle(Turtle turtle){
    Vector2 new = Vector2Add(turtle.pos, turtle.heading);
    DrawLineV(turtle.pos, new, WHITE);
    turtle.pos = new;
    return turtle;
}


