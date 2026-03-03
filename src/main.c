#include "raylib.h"
#include <stdbool.h>
#include <string.h>
#include "turtle/turtle.h"
#include "ui/common.h"
#include "ui/module.h"
#include "ui/ui.h"
#include "ui/custom.h"
#include "clay/clay_renderer_raylib.h"
#include "ui/drawhook.h"

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
    int len = 0;
    bool grabbed = false;
    UpdateReturnValue res = (UpdateReturnValue){false,false};
    while (!WindowShouldClose())    
    {

         flush_draw_hooks();
         update_update_hooks();

         res = update_ui(&clay_context);
         if (!res.grab_mouse && !res.interacted) {
            if (IsKeyDown(KEY_RIGHT)) {
                 camera.target.x += 3.0f;
            }else if (IsKeyDown(KEY_LEFT)) {
                camera.target.x -= 3.0f;
            }else if (IsKeyDown(KEY_DOWN)) {
                camera.target.y += 3.0f;
            }else if (IsKeyDown(KEY_UP)) {
                camera.target.y -= 3.0f;
            }else if (IsKeyDown(KEY_P)) {
                camera.zoom += 0.2f;
            }else if (IsKeyDown(KEY_M)){
                camera.zoom -= 0.2f;
            }
        }
        Clay_RenderCommandArray layout = mk_layout(clay_context);
        BeginDrawing();
        ClearBackground(BLACK);
        Clay_Raylib_Render(layout, clay_context.fonts, NULL);
        draw_module_connections();
        BeginMode2D(camera);
            draw_draw_hooks();
        EndMode2D();
        EndDrawing();
    }
    Clay_Raylib_Close();
    return 0; 
}

