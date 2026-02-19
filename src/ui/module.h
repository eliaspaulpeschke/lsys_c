#ifndef MODULE_H
#define MODULE_H
#include <stdbool.h>
#include "raylib.h"
#include "../turtle/turtle.h"
#include "../lsystem/lsystem.h"

#define MAX_MODULES 128
#define MAX_CONN_POINTS 16

typedef struct Module_input Module_input;
typedef struct Module_output Module_output;
typedef struct Module Module;

typedef struct {
    bool active;
    Vector2 start;
    Vector2 points[MAX_CONN_POINTS];
    unsigned int num_points;
} Connection_drawdata; 

typedef enum {
    MODULE_DATA_TYPE_turtle
  , MODULE_DATA_TYPE_ruleset
  , MODULE_DATA_TYPE_lstring
  , MODULE_DATA_TYPE_integer
  , MODULE_DATA_TYPE_floating
  , MODULE_DATA_TYPE_string
} MODULE_DATA_TYPE;

struct Module_input {
    Module * connection;
    Connection_drawdata * connection_draw_data;
};

struct Module_output {
    bool valid;
    union {
      Turtle turtle;
      LRuleset ruleset;
      LString lstring;
      int integer;
      float floating;
      char * string;
   };
};


typedef enum {
    MOD_CONN_STATUS_IDLE
  , MOD_CONN_STATUS_CONNECTING
} MOD_CONN_STATUS;

typedef enum {
    MODULE_NONE
  , MODULE_INPUT
  , MODULE_OUTPUT
} MODULE_TYPE;

struct Module {
    MODULE_TYPE type;
    MODULE_DATA_TYPE data_type;
    unsigned int clay_id_num;
    union {
      Module_input input;
      Module_output output;
    };
};

#define ERR_MODULE (Module){MODULE_NONE}

void setup_connection_drawdata();
Module mk_module(MODULE_TYPE type, MODULE_DATA_TYPE data_type); 
void draw_module_connections();
bool update_module_connections();
bool update_connection_status();
bool update_module(Module * mod);
void layout_module(Module mod);
#endif
