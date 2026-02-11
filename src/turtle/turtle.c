#include "string.h"
#include <stdlib.h>
#include <stdio.h>
#include "turtle.h"
#include <raymath.h>

void rotate_turtle(Turtle * turtle, bool ccw){
    if (ccw) {
        turtle->heading = Vector2Rotate(turtle->heading, turtle->rads);
    } else { 
        turtle->heading = Vector2Rotate(turtle->heading, -turtle->rads);   }
}

void move_turtle(Turtle * turtle){
    Vector2 new = Vector2Add(turtle->pos, Vector2Scale(turtle->heading, turtle->length));
    DrawLineV(turtle->pos, new, turtle->color);
    turtle->pos = new;
}

void move_turtle_col(Turtle * turtle, Color col){
    Vector2 new = Vector2Add(turtle->pos, Vector2Scale(turtle->heading, turtle->length));
    DrawLineV(turtle->pos, new, col);
    turtle->pos = new;
}

void turtle_set_color(Turtle * turtle, Color col){
    if (turtle != NULL) {
        turtle->color = col;
    }
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

Turtle mk_base_turtle(){
  return (Turtle){ .heading = (Vector2){ 5.0f, 0.0f }
                    , .length = 5.0f
                    , .pos = (Vector2){0.0f, 0.0f}
                    , .prev = NULL
                    , .rads = PI/8
                    , .color = WHITE
                    };
}

void turtle_draw(char * text, unsigned int len, Turtle turtle){
  Turtle * turt = malloc(sizeof(Turtle));
  memcpy(turt, &turtle, sizeof(Turtle));
  if (len > strlen(text)) len = strlen(text);
  for (int i = 0; i < len; i++) {
      switch (text[i]) {
          case 'F':
              move_turtle(turt);
              break;
          case '+':
              rotate_turtle(turt, true);
              break;
          case '-':
              rotate_turtle(turt, false);
              break;
          case '[':
              turt = push_turtle(turt); 
              break;
          case ']':
              turt = pop_turtle(turt);
              break;
          case 'r':
              if (turt->color.r > 9) turt->color.r -= 10;
              break;
          case 'g':
              if (turt->color.g > 9) turt->color.g -= 10;
              break;
          case 'b':
              if (turt->color.b > 9) turt->color.b -= 10;
              break;
          case 'R':
              if (turt->color.r < 246) turt->color.r += 10;
              break;
          case 'G':
              if (turt->color.g < 246) turt->color.g += 10;
              break;
          case 'B':
              if (turt->color.b < 246) turt->color.b += 10;
              break;
          default: 
              break;
      }
   }
  free(turt);
}

void standard_turtle_draw(char * text, Color col, float rads, Vector2 pos){ 
  Turtle turtle = mk_base_turtle();
  turtle_draw(text, strlen(text), turtle);
}
