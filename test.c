#include <ctype.h>
#include <stdio.h>
#include <raylib.h>
#include <string.h>
#include "lib/keys.h"

int main(){
    InitWindow(600, 600, "testing");
    SetTargetFPS(60);
    char buf[5][64];
    for (int i = 0; i < 5; i++){
        memset(buf[i], 0, 64);
    }

    int p = 0;
    int keys[5];
    while (!WindowShouldClose()){
        for (int i = 0; i < 5; i++){
            int k = GetCharPressed(); 
            if (k != '\0') {
                if (i == 0){
                for (int j = 1; j < 5; j++){
                    memset(buf[j], 0, 64);
                }
                }
                keys[i] = k;
                buf[i][0] = k;
//                strcpy(buf[i], kc_to_rep(k));
            } 
        }
        BeginDrawing();

        ClearBackground(BLACK);
        for (int i = 0; i < 5; i++){
            DrawText(buf[i], 20, 20 + 80 * i, 20, WHITE);
        }
        EndDrawing();

    }
    CloseWindow();
    return 0;
}
