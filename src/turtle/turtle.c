#include "string.h"
#include <raylib.h>
#include <stdlib.h>
#include <stdio.h>
#include "turtle.h"
#include "../lsystem/lsystem.h"
#include <raymath.h>

typedef enum {
      DRAW_MODE_2D_LINE
    , DRAW_MODE_2D_BEZIER
    , DRAW_MODE_2D_CATMULL
}TurtleDrawMode2D ;

typedef enum {
      DRAW_MODE_3D_LINE
}TurtleDrawMode3D ;


void draw_mode_2D(TurtleDrawMode2D mode, Vector2 pos, Vector2 new, Vector2 positionQueue[4], float thick, Color color){
    switch (mode) {
        case DRAW_MODE_2D_LINE:
            DrawLineEx(pos, new, thick,color);
            break;
        case DRAW_MODE_2D_BEZIER:
            DrawSplineSegmentCatmullRom(positionQueue[1], positionQueue[2], pos, new, thick, color);
            break;
        case DRAW_MODE_2D_CATMULL:
            DrawSplineSegmentCatmullRom(positionQueue[1], positionQueue[2], pos, new, thick, color);
            break;
    }
}

void draw_mode_3D(TurtleDrawMode3D mode, Vector3 pos, Vector3 new, Vector3 positionQueue[4], float thick, Color color){
    switch (mode) {
        case DRAW_MODE_3D_LINE:
            DrawLine3D(pos, new, color);
            break;
    }
}

void push_pos_queue(Turtle * turtle){
        turtle->positionQueue[0] = turtle->positionQueue[1];
        turtle->positionQueue[1] = turtle->positionQueue[2];
        turtle->positionQueue[2] = turtle->positionQueue[3];
        turtle->positionQueue[3] = turtle->pos;
}

void push_pos_queue_3D(Turtle3D * turtle){
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

void rotate_turtle_3D(Turtle3D * turtle, bool ccw){
        turtle->heading = Vector3RotateByAxisAngle( turtle->heading, turtle->up
                                                  , ccw ? turtle->rads : -turtle->rads );
}

void pitch_turtle_3D(Turtle3D * turtle, double degs){
    Vector3 axis = Vector3CrossProduct(turtle->heading, turtle->up);
    turtle->heading = Vector3RotateByAxisAngle(turtle->heading, axis, degs);
    turtle->up = Vector3RotateByAxisAngle(turtle->up, axis, degs);
}

void yaw_turtle_3D(Turtle3D * turtle, double degs){
    turtle->heading = Vector3RotateByAxisAngle(turtle->heading, turtle->up, degs);
}

void roll_turtle_3D(Turtle3D * turtle, double degs){
    turtle->up = Vector3RotateByAxisAngle(turtle->up, turtle->heading, degs);
}

void rotate_turtle_3D_degs(Turtle3D * turtle, double degs){
        turtle->heading = Vector3RotateByAxisAngle( turtle->heading, turtle->up
                                                  , (float)degs );
}

void rotate_turtle_3D_axis_angle(Turtle3D * turtle, Vector3 axis, float angle){
        turtle->heading = Vector3RotateByAxisAngle(turtle->heading, axis, angle);
        turtle->up = Vector3RotateByAxisAngle(turtle->up, axis, angle);
}

void rotate_turtle_degs(Turtle * turtle, double degs){
        turtle->heading = Vector2Rotate(turtle->heading, (float)degs);
}

void move_turtle(Turtle * turtle){
    Vector2 new = Vector2Add(turtle->pos, Vector2Scale(turtle->heading, turtle->length));
    draw_mode_2D(DRAW_MODE_2D_LINE, turtle->pos, new, turtle->positionQueue, 1, turtle->color);
    turtle->pos = new;
    push_pos_queue(turtle);
}

void move_turtle_len(Turtle * turtle, double len){
    Vector2 new = Vector2Add(turtle->pos, Vector2Scale(turtle->heading, len));
    draw_mode_2D(DRAW_MODE_2D_LINE, turtle->pos, new, turtle->positionQueue, 1, turtle->color);
    turtle->pos = new;
    push_pos_queue(turtle);
}

void move_turtle_3D(Turtle3D * turtle){
    Vector3 new = Vector3Add(turtle->pos, Vector3Scale(turtle->heading, turtle->length));
    draw_mode_3D(DRAW_MODE_3D_LINE, turtle->pos, new, turtle->positionQueue, 1, turtle->color);
    turtle->pos = new;
    push_pos_queue_3D(turtle);
}

void move_turtle_len_3D(Turtle3D * turtle, double len){
    Vector3 new = Vector3Add(turtle->pos, Vector3Scale(turtle->heading, len));
    draw_mode_3D(DRAW_MODE_3D_LINE, turtle->pos, new, turtle->positionQueue, 1, turtle->color);
    turtle->pos = new;
    push_pos_queue_3D(turtle);
}

void turtle_set_color(Turtle * turtle, Color col){
    if (turtle != NULL) {
        turtle->color = col;
    }
}

void turtle_set_color_3D(Turtle3D * turtle, Color col){
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

Turtle3D * push_turtle_3D(Turtle3D * turtle){
    Turtle3D * t = malloc(sizeof(Turtle3D));
    memcpy(t, turtle, sizeof(Turtle3D));
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

Turtle3D * pop_turtle_3D(Turtle3D * turtle){
    if (turtle->prev == NULL) {
        return turtle;
    }
    Turtle3D * prev = turtle->prev;
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

Turtle3D mk_base_turtle_3D(){
  return (Turtle3D){ .heading = (Vector3){ 0.0f, 0.0f, 1.0f }
                    , .up = (Vector3) {0.0f,1.0f,0.0f } 
                    , .length = 5.0f
                    , .pos = (Vector3){0.0f, 0.0f, 0.0f}
                    , .prev = NULL
                    , .rads = PI/8
                    , .color = WHITE
                    , .positionQueue = { (Vector3){0.0f,0.0f, 0.0f}
                                       , (Vector3){0.0f,0.0f, 0.0f}
                                       , (Vector3){0.0f,0.0f, 0.0f}
                                       , (Vector3){0.0f,0.0f, 0.0f}
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

void turtle_draw_3D(LString * text, unsigned int len, Turtle3D turtle){
  Turtle3D * turt = malloc(sizeof(Turtle3D));
  memcpy(turt, &turtle, sizeof(Turtle3D));
  if (len > text->length) len = text->length;
  for (int i = 0; i < text->length; i++) {
      LWord cnt = text->content[i];
      switch (cnt.name) {
          case 'F':
              if (cnt.num_values > 0){
                  move_turtle_len_3D(turt, cnt.values[0]);
              } else {
                  move_turtle_3D(turt);
              }
              break;
          case '+':
              if (cnt.num_values > 0){
                  rotate_turtle_3D_degs(turt, cnt.values[0]);
              } else {
                  rotate_turtle_3D(turt, true);
              }
              break;
          case '-':
              if (cnt.num_values > 0){
                  rotate_turtle_3D_degs(turt, -cnt.values[0]);
              } else {
                  rotate_turtle_3D(turt, false);
              }
              break;
          case '~':
              if (cnt.num_values == 4){
                  rotate_turtle_3D_axis_angle(turt, (Vector3){cnt.values[0],cnt.values[1],cnt.values[2]}, cnt.values[3]);
              }
          case 'p':
               if (cnt.num_values > 0){
                  pitch_turtle_3D(turt, cnt.values[0]);
              } else {
                  pitch_turtle_3D(turt, turt->rads);
              }
              break;
          case 'y':
               if (cnt.num_values > 0){
                  yaw_turtle_3D(turt, cnt.values[0]);
              } else {
                  yaw_turtle_3D(turt, turt->rads);
              }
              break;
          case 'r':
               if (cnt.num_values > 0){
                  roll_turtle_3D(turt, cnt.values[0]);
              } else {
                  roll_turtle_3D(turt, turt->rads);
              }
              break;
          case 'P':
               if (cnt.num_values > 0){
                  pitch_turtle_3D(turt, -cnt.values[0]);
              } else {
                  pitch_turtle_3D(turt, -turt->rads);
              }
              break;
          case 'Y':
               if (cnt.num_values > 0){
                  yaw_turtle_3D(turt, -cnt.values[0]);
              } else {
                  yaw_turtle_3D(turt, -turt->rads);
              }
              break;
          case 'R':
               if (cnt.num_values > 0){
                  roll_turtle_3D(turt, -cnt.values[0]);
              } else {
                  roll_turtle_3D(turt, -turt->rads);
              }
              break;
          case '[':
              turt = push_turtle_3D(turt); 
              break;
          case ']':
              turt = pop_turtle_3D(turt);
              break;
          case 'C':
              if (cnt.num_values == 1){
                  turtle_set_color_3D(turt, (Color){.r = cnt.values[0], .g = cnt.values[0], .b = cnt.values[0], .a=255});
              } else if (cnt.num_values == 3){
                  turtle_set_color_3D(turt, (Color){.r = cnt.values[0], .g = cnt.values[1], .b = cnt.values[2], .a=255});
              } else if (cnt.num_values == 4){
                  turtle_set_color_3D(turt, (Color){.r = cnt.values[0], .g = cnt.values[1], .b = cnt.values[2], .a=cnt.values[3]});
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

void standard_turtle_draw_3D(LString* text){ 
  Turtle3D turtle = mk_base_turtle_3D();
  turtle_draw_3D(text, text->length, turtle);
}
