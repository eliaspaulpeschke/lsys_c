
#include "lib/ui/custom.h"
#include "raylib.h"
#include "raymath.h"
//#include <math.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "lib/lsystem.h"
#include "lib/lsystem_parser.h"
#include "lib/turtle.h"

#include "lib/ui/ui.h"

int main(void)
{
    SetTraceLogLevel(LOG_ALL);
    const int screenWidth = 1920;
    const int screenHeight = 1080;

//    InitWindow(screenWidth, screenHeight, "testing");
    Clay_Raylib_Initialize(screenWidth, screenHeight, "Aristid", FLAG_VSYNC_HINT | FLAG_MSAA_4X_HINT);
//
    SetTargetFPS(60);

    clay_ctx clay_context = init_clay();
    Camera2D camera = { 0 };
    camera.target = (Vector2){ 0.0f, 0.0f};
    camera.offset = (Vector2){ screenWidth/2.0f, screenHeight/2.0f };
    camera.rotation = 0.0f;
    camera.zoom = 1.0f;

    float rads = PI/3;
    Vector2 pos = (Vector2){0.0f, 0.0f};

    while (!WindowShouldClose())    
    {
        
        Clay_RenderCommandArray layout = mk_layout(clay_context);
        if (!update_ui(&clay_context)) {
            if (IsKeyDown(KEY_RIGHT)) {
                 camera.target.x += 3.0f;
            } else if (IsKeyDown(KEY_LEFT)) {
                camera.target.x -= 3.0f;
            }else if (IsKeyDown(KEY_DOWN)) {
                camera.target.y += 3.0f;
            }else if (IsKeyDown(KEY_UP)) {
                camera.target.y -= 3.0f;
            }else if (IsKeyDown(KEY_P)) {
                camera.zoom += 0.2f;
            }else if (IsKeyDown(KEY_M)){
                camera.zoom -= 0.2f;
            }else if (IsKeyDown(KEY_Q)){
                rads += 0.2f;
            }else if (IsKeyDown(KEY_W)){
                rads -= 0.2f;
            }
        }
        BeginDrawing();
        ClearBackground(BLACK);
        Clay_Raylib_Render(layout, clay_context.fonts, NULL);
        BeginMode2D(camera);
        for(int i = 0; i < clay_context.num_custom_elems; i++){
            if (clay_context.ced[i]->type != CUSTOM_ELEM_T_textbox) continue; 
            if (clay_context.ced[i]->textbox.generated == NULL) continue;
            standard_turtle_draw(clay_context.ced[i]->textbox.generated, WHITE, rads, pos);
        }
        EndMode2D();
        EndDrawing();
    }
    Clay_Raylib_Close();
    return 0; 
}

