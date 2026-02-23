#include "string.h"
#include <raylib.h>
#include <stdlib.h>
#include <stdio.h>
#include "turtle.h"
#include "../lsystem/lsystem.h"
#include <raymath.h>

void push_pos_queue(Turtle * turtle){
    turtle->positionQueue[0] = turtle->positionQueue[1];
    turtle->positionQueue[1] = turtle->positionQueue[2];
    turtle->positionQueue[2] = turtle->positionQueue[3];
    turtle->positionQueue[3] = turtle->pos;
}

void rotate_turtle(Turtle * turtle, bool ccw){
    if (ccw) {
        turtle->heading = Vector2Rotate(turtle->heading, turtle->rads);
    } else { 
        turtle->heading = Vector2Rotate(turtle->heading, -turtle->rads);   }
}

void rotate_turtle_degs(Turtle * turtle, double degs){
        turtle->heading = Vector2Rotate(turtle->heading, degs);
}

void move_turtle_len(Turtle * turtle, double len){
    Vector2 new = Vector2Add(turtle->pos, Vector2Scale(turtle->heading, len));
//    DrawLineV(turtle->pos, new, turtle->color);
    DrawSplineSegmentBezierCubic(turtle->positionQueue[2], turtle->positionQueue[1],  new, turtle->pos, 1, turtle->color);
    turtle->pos = new;
    push_pos_queue(turtle);
}

void move_turtle(Turtle * turtle){
    Vector2 new = Vector2Add(turtle->pos, Vector2Scale(turtle->heading, turtle->length));
    DrawSplineSegmentCatmullRom(turtle->positionQueue[1], turtle->positionQueue[2], turtle->pos, new, 1, turtle->color);
//    DrawSplineSegmentBezierCubic(turtle->positionQueue[1], turtle->positionQueue[2], turtle->pos, new, 1, turtle->color);
//    
//    DrawLineV(turtle->pos, new, turtle->color);
    turtle->pos = new;
    push_pos_queue(turtle);

}

void move_turtle_col(Turtle * turtle, Color col){
    Vector2 new = Vector2Add(turtle->pos, Vector2Scale(turtle->heading, turtle->length));
    DrawSplineSegmentBezierCubic(turtle->positionQueue[2], turtle->positionQueue[1],  new, turtle->pos, 1, turtle->color);
 //   DrawLineV(turtle->pos, new, col);
    turtle->pos = new;
    push_pos_queue(turtle);
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
                    , .positionQueue = { (Vector2){0.0f, 0.0f}
                                       , (Vector2){0.0f, 0.0f}
                                       , (Vector2){0.0f, 0.0f}
                                       , (Vector2){0.0f, 0.0f}
                                       }
                    };
}
void turtle_draw(LString * text, unsigned int len, Turtle turtle){
  Turtle * turt = malloc(sizeof(Turtle));
  memcpy(turt, &turtle, sizeof(Turtle));
  if (len > text->length) len = text->length;
  for (int i = 0; i < text->length; i++) {
      LWord cnt = text->content[i];
      switch (cnt.name) {
          case 'F':
              if (cnt.num_values > 0){
                  move_turtle_len(turt, cnt.values[0]);
              } else {
                  move_turtle(turt);
              }
              break;
          case '+':
              if (cnt.num_values > 0){
                  rotate_turtle_degs(turt, cnt.values[0]);
              } else {
                  rotate_turtle(turt, true);
              }
              break;
          case '-':
              if (cnt.num_values > 0){
                  rotate_turtle_degs(turt, -cnt.values[0]);
              } else {
                  rotate_turtle(turt, false);
              }
              break;
          case '[':
              turt = push_turtle(turt); 
              break;
          case ']':
              turt = pop_turtle(turt);
              break;
          case 'C':
              if (cnt.num_values == 1){
                  turtle_set_color(turt, (Color){.r = cnt.values[0], .g = cnt.values[0], .b = cnt.values[0], .a=255});
              } else if (cnt.num_values == 3){
                  turtle_set_color(turt, (Color){.r = cnt.values[0], .g = cnt.values[1], .b = cnt.values[2], .a=255});
              } else if (cnt.num_values == 4){
                  turtle_set_color(turt, (Color){.r = cnt.values[0], .g = cnt.values[1], .b = cnt.values[2], .a=cnt.values[3]});
              }
              break;
          default: 
              break;
      }
   }
  free(turt);
}

void standard_turtle_draw(LString* text){ 
  Turtle turtle = mk_base_turtle();
  turtle_draw(text, text->length, turtle);
}
