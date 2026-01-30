
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

#include "ui.c"

int main(void)
{
    SetTraceLogLevel(LOG_ALL);
    const int screenWidth = 1280;
    const int screenHeight = 720;

//    InitWindow(screenWidth, screenHeight, "testing");
    Clay_Raylib_Initialize(screenWidth, screenHeight, "Aristid", FLAG_WINDOW_HIGHDPI | FLAG_VSYNC_HINT);
//
    SetTargetFPS(60);

    int pressed_chars[16];
    int num_pressed_chars;
    
    clay_ctx clay_context = init_clay();
    while (!WindowShouldClose())    
    {
        for (int i = 0; i < 16; i++){
            pressed_chars[i] = GetCharPressed();
            if (pressed_chars[i] == 0) {
                num_pressed_chars = i;
                break;
            }
        }

        Clay_RenderCommandArray layout = mk_layout(clay_context);
        
        BeginDrawing();
        ClearBackground(BLACK);
        Clay_Raylib_Render(layout, clay_context.fonts);
        EndDrawing();
    }
    Clay_Raylib_Close();
    return 0; 
}

