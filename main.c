
#include "raylib.h"
#include "raymath.h"
#include <math.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "lib/lsystem.h"
#include "lib/lsystem_parser.h"
#include "lib/turtle.h"


int main(void)
{

    Lsystem sys = lsystem_from_file("test");

    char * res = applyRules(sys.ruleset, sys.nrules, sys.axiom);
    res = applyRules(sys.ruleset, sys.nrules, res);
    res = applyRules(sys.ruleset, sys.nrules, res);


    const int screenWidth = 600;
    const int screenHeight = 600;

    SetWindowState(FLAG_WINDOW_HIGHDPI | FLAG_VSYNC_HINT);
//

    Camera2D camera = {0};
    camera.target = (Vector2){0.0f, 0.0f};
    camera.offset = (Vector2){0.0f, 0.0f};
    camera.rotation = 0.0f;
    camera.zoom = 1.0f;

    InitWindow(screenWidth, screenHeight, "fun");
    SetTargetFPS(60);

    Turtle turtle = { (Vector2){300.0f, 300.0f}
                    , (Vector2){1.0f, 0.0f}
                    , 1.0f
                    , PI / 16.0f };

    while (!WindowShouldClose())    
    {

        BeginDrawing();
        ClearBackground(BLACK);
        float delta = 0.7f;

        if (IsKeyDown(KEY_LEFT)) {
                camera.target.x = camera.target.x + delta;
        }
        if (IsKeyDown(KEY_RIGHT)) {
                camera.target.x = camera.target.x - delta;
        }
        if (IsKeyDown(KEY_UP)) {
                camera.target.y = camera.target.y + delta;
        }
        if (IsKeyDown(KEY_DOWN)) {
                camera.target.y = camera.target.y - delta;
        }

        BeginMode2D(camera);

        Turtle turtle = { (Vector2){300.0f, 300.0f}
                    , (Vector2){10.0f, 0.0f}
                    , 1.0f
                    , PI / 3.0f };

        for (uint i = 0; i < strlen(res); i++) {
            switch (res[i]) {
                case 'F':
                    turtle = moveTurtle(turtle);
                    break;
                case '+':
                    turtle = rotateTurtle(turtle, true);
                    break;
                case '-':
                    turtle = rotateTurtle(turtle, false);
                    break;
                default:
                    break;
            }
        } 
        
        EndMode2D();
        EndDrawing();
    }
    CloseWindow();   
    return 0;
}

