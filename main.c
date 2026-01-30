
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

Color getCol(int i){
    switch (i){
        case 1:
            return WHITE;
        case 2:
            return RED;
        case 3: 
            return GREEN;
        case 4:
            return BLUE;
        default:
            return YELLOW;
    }
}

void test(char * str){
    int open = 0;
    for(int i = 0; i < strlen(str); i++){
        switch (str[i]) {
            case '[':
                open++;
                break;
            case ']':
                open--;
                break;
            default:
                break;
        }
        if (open < 0) printf("unbalanced: %d of %lu \n",i, strlen(str));
    }
}


int main(void)
{

    Lsystem sys = lsystem_from_file("test", true);

    char * res = applyRules(sys.ruleset, sys.nrules, sys.axiom, false);
    res = applyRules(sys.ruleset, sys.nrules, res, true);
    res = applyRules(sys.ruleset, sys.nrules, res, true);
    res = applyRules(sys.ruleset, sys.nrules, res, true);

    for (int i = 0; i < sys.nrules; i++){
        print_rule(&sys.ruleset[i], "  ");
    }

    printf("string length: %lu \n", strlen(res));

    test(res);

    printf("\n\"%s\"\n", res);

    const int screenWidth = 600;
    const int screenHeight = 600;

    int col = 1;

    SetWindowState(FLAG_WINDOW_HIGHDPI | FLAG_VSYNC_HINT);

    Camera2D camera = {0};
    camera.target = (Vector2){0.0f, 0.0f};
    camera.offset = (Vector2){0.0f, 0.0f};
    camera.rotation = 1.0f;
    camera.zoom = 1.0f;

    InitWindow(screenWidth, screenHeight, "fun");
    SetTargetFPS(60);

    Turtle * turtle = malloc(sizeof(Turtle));
    *turtle = (Turtle){ (Vector2){300.0f, 300.0f}
                , (Vector2){10.0f, 0.0f}
                , 1.0f
                , PI / 3.0f };    

    while (!WindowShouldClose())    
    {
        float delta = 1.5f;
        float delta_zoom = 1.05f;

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
        if (IsKeyDown(KEY_P)) {
                camera.zoom *= delta_zoom;
        }
        if (IsKeyDown(KEY_M)) {
                camera.zoom /= delta_zoom;
        }
        if (IsKeyDown(KEY_W)) {
            turtle->rads += 0.1f;
        }
        if (IsKeyDown(KEY_Q)) {
            turtle->rads -= 0.1f;
        }

        turtle->pos = (Vector2){300.0f, 300.0f};
        turtle->heading = (Vector2){10.0f, 0.0f};

        BeginDrawing();
        ClearBackground(BLACK);
        BeginMode2D(camera);

        for (uint i = 0; i < strlen(res); i++) {
            switch (res[i]) {
                case 'F':
                    moveTurtleCol(turtle, getCol(col));
                    break;
                case '+':
                    rotateTurtle(turtle, true);
                    break;
                case '-':
                    rotateTurtle(turtle, false);
                    break;
                case '[':
                    col +=1;
                    turtle = pushTurtle(turtle);
                    break;
                case ']':
                    col -=1;
                    turtle = popTurtle(turtle);
                    break;
                default:
                    break;
            }
        } 
        EndMode2D();
        EndDrawing();
    }
    free_lsystem(sys);
    while (turtle->prev != NULL) {
        turtle = popTurtle(turtle);
    }
    free(turtle);
    free(res);
    CloseWindow();   
    return 0; 
}

