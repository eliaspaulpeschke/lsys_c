
#include "raylib.h"
#include "raymath.h"
//#include <math.h>
#include <stdbool.h>
/*#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "lib/lsystem.h"
#include "lib/lsystem_parser.h"
#include "lib/turtle.h"*/

#include "lib/ui/ui.h"

int main(void)
{
    SetTraceLogLevel(LOG_ALL);
    const int screenWidth = 1280;
    const int screenHeight = 720;

//    InitWindow(screenWidth, screenHeight, "testing");
    Clay_Raylib_Initialize(screenWidth, screenHeight, "Aristid", FLAG_WINDOW_HIGHDPI | FLAG_VSYNC_HINT);
//
    SetTargetFPS(60);

    clay_ctx clay_context = init_clay();
    while (!WindowShouldClose())    
    {
        Clay_RenderCommandArray layout = mk_layout(clay_context);
        
        BeginDrawing();
        ClearBackground(BLACK);
        Clay_Raylib_Render(layout, clay_context.fonts);
        EndDrawing();
    }
    Clay_Raylib_Close();
    return 0; 
}

