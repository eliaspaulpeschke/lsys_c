#include "raylib.h"
#include "raymath.h"
#include <math.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct Rule {
    char premise;
    uint result_len;
    char *result;
} Rule;

Rule mkRule (char premise, uint result_len, char * result) {
    char *the_res = malloc((result_len) * sizeof(char));
    Rule the_rule = {premise, result_len, the_res};
    strcpy(the_rule.result, result);
    return the_rule;
}

typedef struct Lstring {
    uint length;
    char *str;
} Lstring;

typedef struct Lsystem {
    uint nrules;
    Lstring init;
    Rule *ruleset;
} Lsystem;

Lstring applyRules( Rule rules[], uint n_rules, Lstring input){
    uint output_len = input.length << 1;
    uint output_pos = 0;
    char * the_output = malloc(output_len);
    for(uint i = 0; i < input.length; i++){
        char prem = input.str[i];
        bool copyflag = true;
        for (uint j = 0; j < n_rules; j++){
            Rule current_rule = rules[j];
            if (prem == current_rule.premise) {
                while (output_len <= 
                        output_pos + current_rule.result_len) {
                    output_len = output_len << 1;
                    the_output = realloc(the_output, output_len);
                }
                memcpy(the_output + output_pos, current_rule.result, current_rule.result_len - 1);
                output_pos += current_rule.result_len -1;
                copyflag = false;

            }
        }
        if (copyflag) {
            if (output_pos < output_len) {
              the_output[output_pos] = prem;
              output_pos += 1;
            }
        }
    }
    /* THIS IS THE WRONG LENGTH */
    Lstring res = {output_len, the_output};
    return res;
}

enum RULE_STATE {
    RS_INIT
  , RS_RULES
};

enum PARSE_STATE {
    PS_PREMISE
  , PS_EQ
  , PS_RESULT
  , PS_FIN
};

Lsystem readRuleFile(char * fname){
    enum RULE_STATE state = RS_INIT;
    enum PARSE_STATE pstate = PS_PREMISE;
    FILE* rulefile = fopen(fname, "r");
    if (rulefile == NULL) return (Lsystem){NULL, NULL, NULL};
    char line[512];
    uint nrules = 0;
    uint maxrules = 0;
    Rule * rules = malloc(8 * sizeof(Rule));
    Lstring init;

    while (fgets(line, sizeof(line), rulefile)) {
        switch (state) {
            case RS_INIT: ;
                uint l = 0;
                for (int i = 0; i < 512; i++){
                    if (strcmp(&line[i], "-")) {
                        state = RS_RULES;
                        goto reiterate;
                    }
                    if (strcmp(&line[i], "\n")) {
                        init.length = i;
                        init.str = malloc(sizeof(char) * i);
                        memcpy(init.str, line, sizeof(char) * (i-1));
                        init.str[i] = '\0';
                    }
                }
                break;
            case RS_RULES:
                pstate = PS_PREMISE;
                while (pstate != PS_FIN) {
                    switch (pstate) {
                        case PS_PREMISE: 
                            break;
                        case PS_EQ:
                            break;
                        case PS_RESULT:
                            break;
                        default: 
                            goto reiterate;
                }
                break;
        }
        reiterate: ;
    }
    fclose(rulefile);
}

typedef struct Turtle {
    Vector2 pos;
    Vector2 heading;
    float length;
    float rads;
} Turtle;

Turtle rotateTurtle(Turtle turtle, bool ccw){
    if (ccw) {
        turtle.heading = Vector2Rotate(turtle.heading, turtle.rads);
    } else { 
        turtle.heading = Vector2Rotate(turtle.heading, -turtle.rads);   }
    return turtle;
}

Turtle moveTurtle(Turtle turtle){
    Vector2 new = Vector2Add(turtle.pos, turtle.heading);
    DrawLineV(turtle.pos, new, WHITE);
    turtle.pos = new;
    return turtle;
}

int main(void)
{
    Lstring test = {8, "F++F++F"};
    Rule testrule = mkRule('F', 9, "F-F++F-F");

    Lstring test2 = applyRules(&testrule, 1, test);
    test2 = applyRules(&testrule, 1, test2);
    test2 = applyRules(&testrule, 1, test2);
    test2 = applyRules(&testrule, 1, test2);

    puts(test2.str);

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
                    , (Vector2){3.0f, 0.0f}
                    , 1.0f
                    , PI / 3.0f };


        
        for (uint i = 0; i < test2.length; i++) {
            switch (test2.str[i]) {
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

