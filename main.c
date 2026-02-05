
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
    Clay_Raylib_Initialize(screenWidth, screenHeight, "Aristid", FLAG_WINDOW_HIGHDPI | FLAG_VSYNC_HINT);
//
    SetTargetFPS(60);

    clay_ctx clay_context = init_clay();
    Camera2D camera = { 0 };
    camera.target = (Vector2){ 0.0f, 0.0f};
    camera.offset = (Vector2){ screenWidth/2.0f, screenHeight/2.0f };
    camera.rotation = 0.0f;
    camera.zoom = 1.0f;

    char * generated = malloc(4096);

    Turtle * turtle = malloc(sizeof(Turtle));

    *turtle = (Turtle){ .heading = (Vector2){ 5.0f, 0.0f }
                      , .length = 5.0f
                      , .pos = (Vector2){ 0.0f, 0.0f }
                      , .prev = NULL
                      , .rads = PI/3 
                      };

    while (!WindowShouldClose())    
    {
        if (IsKeyDown(KEY_RIGHT)) {
             camera.target.x += 1.0f;
        } else if (IsKeyDown(KEY_LEFT)) {
            camera.target.x -= 1.0f;
        }else if (IsKeyDown(KEY_DOWN)) {
            camera.target.y += 1.0f;
        }else if (IsKeyDown(KEY_UP)) {
            camera.target.y -= 1.0f;
        }
        Clay_RenderCommandArray layout = mk_layout(clay_context);
        
        BeginDrawing();
        ClearBackground(BLACK);
        Clay_Raylib_Render(layout, clay_context.fonts, NULL);
        BeginMode2D(camera);
        if (clay_context.ced->textbox.lsystem != NULL) {
            //TODO: update this only if the underlying lsystem changed
            Lsystem * sys = clay_context.ced->textbox.lsystem;
            generated = applyRules(sys->ruleset, sys->nrules, sys->axiom, false);
            generated = applyRules(sys->ruleset, sys->nrules, generated, true);
            *turtle = (Turtle){ .heading = (Vector2){ 5.0f, 0.0f }
                      , .length = 5.0f
                      , .pos = (Vector2){ 0.0f, 0.0f }
                      , .prev = NULL
                      , .rads = PI/3 
                      };
            for (int i = 0; i < strlen(generated); i++) {
                switch (generated[i]) {
                    case 'F':
                        moveTurtle(turtle);
                        break;
                    case '+':
                        rotateTurtle(turtle, true);
                        break;
                    case '-':
                        rotateTurtle(turtle, false);
                        break;
                    case '[':
                        turtle = pushTurtle(turtle); 
                        break;
                    case ']':
                        turtle = popTurtle(turtle);
                        break;
                    default: 
                        break;
                }
            }
        }
        EndMode2D();
        EndDrawing();
    }
    Clay_Raylib_Close();
    return 0; 
}

