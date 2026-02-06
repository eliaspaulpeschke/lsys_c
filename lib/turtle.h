#ifndef TURTLE_H
#define TURTLE_H
#include "string.h"
#include <raylib.h>
#include <stdlib.h>
#include <stdio.h>
#include <raymath.h>

typedef struct Turtle {
    Vector2 pos;
    Vector2 heading;
    float length;
    float rads;
    struct Turtle * prev;
} Turtle;

void rotate_turtle(Turtle * turtle, bool ccw){
    if (ccw) {
        turtle->heading = Vector2Rotate(turtle->heading, turtle->rads);
    } else { 
        turtle->heading = Vector2Rotate(turtle->heading, -turtle->rads);   }
}

void move_turtle(Turtle * turtle){
    Vector2 new = Vector2Add(turtle->pos, turtle->heading);
    DrawLineV(turtle->pos, new, WHITE);
    turtle->pos = new;
}

void move_turtle_col(Turtle * turtle, Color col){
    Vector2 new = Vector2Add(turtle->pos, turtle->heading);
    DrawLineV(turtle->pos, new, col);
    turtle->pos = new;
}

Turtle * push_turtle(Turtle * turtle){
    Turtle * t = malloc(sizeof(Turtle));
    memcpy(t, turtle, sizeof(Turtle));
    t->prev = turtle;
    return t;
}

Turtle * pop_turtle(Turtle * turtle){
    if (turtle->prev == NULL) {
        return turtle;
    }
    Turtle * prev = turtle->prev;
    free(turtle);
    return prev;
}

void standard_turtle_draw(char * text, Color col, float rads, Vector2 pos){ 
  Turtle * turtle = malloc(sizeof(Turtle));
  if (turtle == NULL) return;
  *turtle = (Turtle){ .heading = (Vector2){ 5.0f, 0.0f }
                    , .length = 5.0f
                    , .pos = pos 
                    , .prev = NULL
                    , .rads = rads 
                    };
  for (int i = 0; i < strlen(text); i++) {
      switch (text[i]) {
          case 'F':
              move_turtle_col(turtle, col);
              break;
          case '+':
              rotate_turtle(turtle, true);
              break;
          case '-':
              rotate_turtle(turtle, false);
              break;
          case '[':
              turtle = push_turtle(turtle); 
              break;
          case ']':
              turtle = pop_turtle(turtle);
              break;
          default: 
              break;
      }
   }
  if (turtle != NULL){
      free(turtle);
  }
}

#endif
