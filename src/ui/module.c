#include "../clay/clay.h"
#include "common.h"
#include "raylib.h"
#include "raymath.h"
#include <stdlib.h>
#include "module.h"

static int NUM_MODULE_IDS = 0; // only count up
static int IDX_MOD_DRAWDATA = 0;

Connection_drawdata connection_drawdata[MAX_MODULES];

void setup_connection_drawdata(){
    memset(connection_drawdata, 0, sizeof(Connection_drawdata) * MAX_MODULES);
}

Clay_String module_type_name(MODULE_DATA_TYPE type){
    switch (type) {
        case MODULE_DATA_TYPE_string: 
            return CLAY_STRING("str");
        case MODULE_DATA_TYPE_ruleset:
            return CLAY_STRING("rul");
        case MODULE_DATA_TYPE_floating:
            return CLAY_STRING("flo");
        case MODULE_DATA_TYPE_axiom:
            return CLAY_STRING("axi");
        case MODULE_DATA_TYPE_integer:
            return CLAY_STRING("int");
        case MODULE_DATA_TYPE_turtle:
            return CLAY_STRING("tur");
        default:
            return CLAY_STRING("nul");
    }
}

Module mk_module(MODULE_TYPE type, MODULE_DATA_TYPE data_type){
    if (NUM_MODULE_IDS >= MAX_MODULES || type == MODULE_NONE) return (Module){ .type = MODULE_NONE };
    unsigned int id = NAMESPACE_MODULE + NUM_MODULE_IDS;
    NUM_MODULE_IDS += 1;
    TraceLog(LOG_DEBUG, "Creating mod %d", id); 
    Module mod =   { .type = type
                   , .data_type = data_type
                   , .clay_id_num = id
                   };
    if (type == MODULE_INPUT) {
        if (IDX_MOD_DRAWDATA >= MAX_MODULES) return (Module){ .type = MODULE_NONE };
        mod.input.connection_draw_data = connection_drawdata + IDX_MOD_DRAWDATA;
        IDX_MOD_DRAWDATA += 1;
    }
    return mod;

}

struct {
    MOD_CONN_STATUS connecting_status;
    Module * module;
} MODULES_CONNECTION_STATUS = { .connecting_status = MOD_CONN_STATUS_IDLE 
                              , .module = NULL
                              };

void handle_module_hover(Clay_ElementId id, Clay_PointerData ptr, void * userData){
    Module * mod = (Module*)(userData);
    TraceLog(LOG_DEBUG, "Hovering mod %d", mod->clay_id_num); 
    if (MODULES_CONNECTION_STATUS.connecting_status == MOD_CONN_STATUS_IDLE) {
        if (IsMouseButtonDown(0)) {
            TraceLog(LOG_DEBUG, "connecting"); 
            MODULES_CONNECTION_STATUS.connecting_status =  MOD_CONN_STATUS_CONNECTING;
            MODULES_CONNECTION_STATUS.module = mod; 
            return;
        }
    } else {
        if (IsMouseButtonReleased(0)){
            TraceLog(LOG_DEBUG, "finishing connection"); 
            if (MODULES_CONNECTION_STATUS.module == NULL){
                MODULES_CONNECTION_STATUS.connecting_status =  MOD_CONN_STATUS_IDLE;
                return;
            }
            if (mod->data_type != MODULES_CONNECTION_STATUS.module->data_type || mod->type == MODULE_NONE) goto reset;
            if (mod->type == MODULE_INPUT){
                if (MODULES_CONNECTION_STATUS.module->type != MODULE_OUTPUT) goto reset;
                mod->input.connection = MODULES_CONNECTION_STATUS.module;

                Clay_ElementData end = Clay_GetElementData(Clay_GetElementIdWithIndex(
                    CLAY_STRING("module"), mod->clay_id_num));
                Clay_ElementData start = Clay_GetElementData(Clay_GetElementIdWithIndex(
                    CLAY_STRING("module"), mod->input.connection->clay_id_num));
                Connection_drawdata * cdd = mod->input.connection_draw_data;

                cdd->start = (Vector2){start.boundingBox.x + start.boundingBox.width / 2
                , start.boundingBox.y + start.boundingBox.height / 2};
                cdd->num_points = 4;
                cdd->points[1] = cdd->start;
                cdd->points[0] = (Vector2){ cdd->start.x - 30.0f, cdd->start.y };
                cdd->points[2] = (Vector2){ end.boundingBox.x + end.boundingBox.width / 2
                , end.boundingBox.y + end.boundingBox.height / 2};
                cdd->points[3] = (Vector2){ cdd->points[2].x - 30.0f, cdd->points[2].y };

                TraceLog(LOG_DEBUG, "conn in->out"); 

                goto reset;
            }else if (mod->type == MODULE_OUTPUT){
                if (MODULES_CONNECTION_STATUS.module->type != MODULE_INPUT) goto reset;
                MODULES_CONNECTION_STATUS.module->input.connection = mod;

                Clay_ElementData start = Clay_GetElementData(Clay_GetElementIdWithIndex(
                    CLAY_STRING("module"), mod->clay_id_num));
                Clay_ElementData end = Clay_GetElementData(Clay_GetElementIdWithIndex(
                    CLAY_STRING("module"), MODULES_CONNECTION_STATUS.module->clay_id_num));
                Connection_drawdata * cdd = MODULES_CONNECTION_STATUS.module->input.connection_draw_data;

                cdd->start = (Vector2){start.boundingBox.x + start.boundingBox.width / 2
                , start.boundingBox.y + start.boundingBox.height / 2};
                cdd->num_points = 4;
                cdd->points[0] = cdd->start;
                cdd->points[1] = (Vector2){ cdd->start.x - 30.0f, cdd->start.y };
                cdd->points[3] = (Vector2){ end.boundingBox.x + end.boundingBox.width / 2
                , end.boundingBox.y + end.boundingBox.height / 2};
                cdd->points[2] = (Vector2){ cdd->points[3].x - 30.0f, cdd->points[3].y };                
                TraceLog(LOG_DEBUG, "conn out->in"); 


                goto reset;
            }

        }
    }
    return;
    reset:
      MODULES_CONNECTION_STATUS.connecting_status =  MOD_CONN_STATUS_IDLE;
      MODULES_CONNECTION_STATUS.module = NULL;
      return;
}

bool update_connection_status(){
    if (MODULES_CONNECTION_STATUS.connecting_status == MOD_CONN_STATUS_IDLE) return false;
    if (IsMouseButtonUp(0)) {
        MODULES_CONNECTION_STATUS.connecting_status = MOD_CONN_STATUS_IDLE;
        return false;
    }
    return true;
}

Clay_String module_kind_name(Module mod){
    switch (mod.type) {
        case MODULE_INPUT:
            return CLAY_STRING("in");
        case MODULE_OUTPUT:
            return CLAY_STRING("out");
        case MODULE_NONE:
            return CLAY_STRING("xx");
        default:
            return CLAY_STRING("xx");
    }
}

bool update_module_connections(){
    bool down = IsMouseButtonDown(0);
    bool rel = IsMouseButtonReleased(0);
    if (rel || down) {
        Vector2 mouse = GetMousePosition();
        Vector2 delta = GetMouseDelta();
        for (int i = 0; i < IDX_MOD_DRAWDATA; i++){
          Connection_drawdata * cdd = &connection_drawdata[i];
          if (!cdd->active) continue;
          if (rel){
              if (Vector2Distance(mouse, cdd->points[cdd->num_points-2]) < 10.0f){
                  if (cdd->num_points < MAX_CONN_POINTS) {
                      cdd->points[cdd->num_points] = cdd->points[cdd->num_points -1];
                      cdd->points[cdd->num_points -1] = cdd->points[cdd->num_points-2];
                      connection_drawdata[i].num_points++;
                      cdd->points[cdd->num_points-3] = Vector2Lerp(cdd->points[0], cdd->points[cdd->num_points -1], 0.5f);
                  }
              }
          }
          else if (down){
              for(int i = 1; i < cdd->num_points -1 ; i++){ //last point is end, first point is start
                  if(Vector2Distance(mouse, cdd->points[i]) < 30.0f){
                      cdd->points[i] = Vector2Add(cdd->points[i], delta);
                      return true;
                  }
              }
          }
        }
    }
    return false;
}


void draw_module_connections(){
    if (MODULES_CONNECTION_STATUS.connecting_status == MOD_CONN_STATUS_CONNECTING) {
        if (MODULES_CONNECTION_STATUS.module == NULL) return;
        if (MODULES_CONNECTION_STATUS.module->type == MODULE_NONE) return;
        Clay_ElementData data = Clay_GetElementData(Clay_GetElementIdWithIndex(
                    CLAY_STRING("module"), MODULES_CONNECTION_STATUS.module->clay_id_num));
        if (!data.found) return;
        Vector2 mouse = GetMousePosition();
        DrawLineEx(mouse, (Vector2){data.boundingBox.x + data.boundingBox.width / 2, data.boundingBox.y + data.boundingBox.height/2},2, GREEN);
    }
    for (int i = 0; i < IDX_MOD_DRAWDATA; i++){
        Connection_drawdata cdd = connection_drawdata[i];
        if (!cdd.active) continue;
        DrawSplineCatmullRom(cdd.points, cdd.num_points, 2, DARKGREEN);
        for(int i = 1; i < cdd.num_points -1; i++){
        DrawCircle(cdd.points[i].x, cdd.points[i].y, 4, GREEN);
        }
    }
}

void layout_module(Module * mod){
    MODULE_DATA_TYPE data; 
    CLAY(CLAY_IDI("module", mod->clay_id_num), {
            .layout = { .sizing = {CLAY_SIZING_FIXED(24), CLAY_SIZING_FIXED(48)}
                      , .padding = CLAY_PADDING_ALL(4)
                      , .layoutDirection = CLAY_TOP_TO_BOTTOM
                      , .childAlignment = {CLAY_ALIGN_X_CENTER, CLAY_ALIGN_Y_CENTER}
                      }
          , .backgroundColor = COL_LIGHT
          }){
        Clay_OnHover(&handle_module_hover, mod); 
        CLAY_TEXT(module_kind_name(*mod)
                 , CLAY_TEXT_CONFIG( {.fontSize = 14, .fontId = 0, .textColor = {0,0,0,255}, .lineHeight = 16.0 }));
        CLAY_AUTO_ID({ .layout = {.sizing = {CLAY_SIZING_FIXED(8), CLAY_SIZING_FIXED(8)}}
                     , .backgroundColor = COL_DARK
                     , .cornerRadius = {8,8,8,8}
                     , .border = {.width = CLAY_BORDER_OUTSIDE(2), .color = COL_ACCENT}});
        CLAY_TEXT(module_type_name(mod->data_type)
                 , CLAY_TEXT_CONFIG( {.fontSize = 14, .fontId = 0, .textColor = {0,0,0,255}, .lineHeight = 16.0 }));
    };

    if (mod->type == MODULE_INPUT && mod->input.connection != NULL) {
        Clay_ElementData data = Clay_GetElementData(Clay_GetElementIdWithIndex(
                    CLAY_STRING("module"), mod->clay_id_num));
        Clay_ElementData data2 = Clay_GetElementData(Clay_GetElementIdWithIndex(
                    CLAY_STRING("module"), mod->input.connection->clay_id_num));
        if (!(data.found && data2.found)) return;
        Connection_drawdata * cdd = mod->input.connection_draw_data;
        cdd->active = true;
        cdd->start = (Vector2){data2.boundingBox.x + data2.boundingBox.width / 2
                , data2.boundingBox.y + data2.boundingBox.height / 2};
        cdd->points[1] = cdd->start;
        cdd->points[0] = (Vector2){ cdd->start.x - 30.0f, cdd->start.y };
        cdd->points[cdd->num_points-2] = (Vector2){ data.boundingBox.x + data.boundingBox.width / 2 
                , data.boundingBox.y + data.boundingBox.height / 2};
        cdd->points[cdd->num_points-1] = (Vector2){ cdd->points[cdd->num_points-2].x - 30.0f
                                                  , cdd->points[cdd->num_points-2].y };
    } else if (mod->type == MODULE_INPUT && mod->input.connection == NULL){
        Connection_drawdata * cdd = mod->input.connection_draw_data;
        cdd->active = false;
    }
}
