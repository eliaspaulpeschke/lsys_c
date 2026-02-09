#include "../clay/clay.h"
#include "common.h"
#include "raylib.h"
#include "raymath.h"
#include "module.h"

static int NUM_MODULE_IDS = 0; // only count up
static int IDX_MOD_DRAWDATA = 0;

Connection_drawdata connection_drawdata[MAX_MODULES];

void setup_connection_drawdata(){
    memset(connection_drawdata, 0, sizeof(Connection_drawdata) * MAX_MODULES);
}

Clay_String module_type_name(MODULE_DATA_TYPE type){
    switch (type) {
        case MODULE_INPUT_TYPE_string: 
            return CLAY_STRING("str");
        case MODULE_INPUT_TYPE_ruleset:
            return CLAY_STRING("rul");
        case MODULE_INPUT_TYPE_floating:
            return CLAY_STRING("flo");
        case MODULE_INPUT_TYPE_axiom:
            return CLAY_STRING("axi");
        case MODULE_INPUT_TYPE_integer:
            return CLAY_STRING("int");
        case MODULE_INPUT_TYPE_turtle:
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

                Clay_ElementData data = Clay_GetElementData(Clay_GetElementIdWithIndex(
                    CLAY_STRING("module"), mod->clay_id_num));
                Clay_ElementData data2 = Clay_GetElementData(Clay_GetElementIdWithIndex(
                    CLAY_STRING("module"), mod->input.connection->clay_id_num));
                Connection_drawdata * cdd = mod->input.connection_draw_data;
                cdd->p1 = (Vector2){data2.boundingBox.x + data2.boundingBox.width / 2
                , data2.boundingBox.y + data2.boundingBox.height / 2};
                cdd->p2 = (Vector2){ data.boundingBox.x + data.boundingBox.width / 2
                , data.boundingBox.y + data.boundingBox.height / 2};

                goto reset;
            }else if (mod->type == MODULE_OUTPUT){
                if (MODULES_CONNECTION_STATUS.module->type != MODULE_INPUT) goto reset;
                MODULES_CONNECTION_STATUS.module->input.connection = mod;


                Clay_ElementData data = Clay_GetElementData(Clay_GetElementIdWithIndex(
                    CLAY_STRING("module"), mod->clay_id_num));
                Clay_ElementData data2 = Clay_GetElementData(Clay_GetElementIdWithIndex(
                    CLAY_STRING("module"), MODULES_CONNECTION_STATUS.module->clay_id_num));

                Connection_drawdata * cdd =MODULES_CONNECTION_STATUS.module->input.connection_draw_data;
                cdd->p2 = (Vector2){data2.boundingBox.x + data2.boundingBox.width / 2
                , data2.boundingBox.y + data2.boundingBox.height / 2};
                cdd->p1 = (Vector2){ data.boundingBox.x + data.boundingBox.width / 2
                , data.boundingBox.y + data.boundingBox.height / 2};


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
    if (IsMouseButtonDown(0)) {
        Vector2 mouse = GetMousePosition();
        Vector2 delta = GetMouseDelta();
        for (int i = 0; i < IDX_MOD_DRAWDATA; i++){
          Connection_drawdata cdd = connection_drawdata[i];
          if (!cdd.active) continue;
          if(Vector2Distance(mouse, cdd.p1) < 10){
              cdd.p1 = Vector2Add(cdd.p1, delta);
              return true;
          }else if(Vector2Distance(mouse, cdd.p2) < 10){
              cdd.p1 = Vector2Add(cdd.p2, delta);
              return true;
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
        DrawSplineSegmentBezierCubic(cdd.start, cdd.p1, cdd.p2, cdd.end, 2, DARKGREEN);
        DrawCircle(cdd.p1.x, cdd.p1.y, 4, GREEN);
        DrawCircle(cdd.p2.x, cdd.p2.y, 4, GREEN);
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
        cdd->end = (Vector2){ data.boundingBox.x + data.boundingBox.width / 2
                , data.boundingBox.y + data.boundingBox.height / 2};
    } else if (mod->type == MODULE_INPUT && mod->input.connection == NULL){
        Connection_drawdata * cdd = mod->input.connection_draw_data;
        cdd->active = false;
    }
}
