#ifndef TEXTBOX_H
#define TEXTBOX_H
#include <stdbool.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <stdlib.h>
#include <raylib.h>
#include "../clay.h"
#include "colors.h"

typedef struct {
    char * text;
    bool changed;
    uint lenText;
    char * bufA;
    char * bufB;
    uint lenA;
    uint lenB;
    uint posA;
    uint posB; //used backwards, start at lenB-1
    uint max_len;
} textbox;

void free_textbox(textbox * tb){
    free(tb->bufA);
    free(tb->bufB);
    free(tb->text);
    free(tb);
}

bool realloc_bufA(textbox * tb){
  TraceLog(LOG_DEBUG, "realloc a");
  if ((tb->lenA << 1) + tb->lenB >= tb->max_len) return false;
  char * temp = realloc(tb->bufA, tb->lenA << 1);
  bool success = false;
  if (temp != NULL) {
      memset(tb->bufA + tb->lenA, '\0', tb->lenA);
      tb->bufA = temp;
      tb->lenA <<= 1;
      success = true;
  } 
  return success;
} 

bool realloc_bufB(textbox * tb){
  TraceLog(LOG_DEBUG, "realloc b");
  if ((tb->lenB << 1) + tb->lenA >= tb->max_len) return false;
  char * temp = malloc(tb->lenB << 1);
  bool success = false;
  if (temp != NULL) {
     memset(temp + tb->lenB, '\0', tb->lenB);
     memcpy(temp + tb->lenB, tb->bufB, tb->lenB);
     free(tb->bufB);
     tb->bufB = temp;
     tb->posB = tb->lenB;
     tb->lenB <<= 1;
     success = true;
  }
  return success;
}

void print_tb(textbox * tb){
    uint lb = tb->lenB - tb->posB;
    char * testA = malloc(tb->posA + 1);
    char * testB = malloc(lb);
    memcpy(testA, tb->bufA, tb->posA);
    memcpy(testB, tb->bufB + tb->posB + 1, lb - 1);
    testA[tb->posA] = '\0';
    testB[lb - 1] = '\0';
    TraceLog(LOG_DEBUG, "A: %s", testA); 
    TraceLog(LOG_DEBUG, "B: %s", testB); 
    free(testA);
    free(testB);
}

void update_textbox(textbox * tb){
    char chr = GetCharPressed();
    KeyboardKey key = GetKeyPressed();
    if (chr != 0){
        if (tb->posA >= tb->lenA) if (!realloc_bufA(tb)) return; 
        tb->bufA[tb->posA] = chr;
        tb->posA += 1;
        tb->changed = true;
        TraceLog(LOG_DEBUG,"charring %d", tb->posA); 
    } else if (key != KEY_NULL) {
        TraceLog(LOG_DEBUG,"keying"); 
        switch (key) {
            case KEY_BACKSPACE:
                if (tb->posA >= 0){
                    TraceLog(LOG_DEBUG,"backspace"); 
                    if (tb->posA > 0) tb->posA -= 1;
                    tb->bufA[tb->posA] = '\0';
                    tb->changed = true;
                    // shrink buffer?
                }
                break;
            case KEY_LEFT:
                if (tb->posA > 0){
                    if (tb->posB == 0) if (!realloc_bufB(tb)) return;
                    tb->posA -= 1;
                    tb->bufB[tb->posB] = tb->bufA[tb->posA];
                    tb->bufA[tb->posA] = '\0';
                    tb->posB -= 1;
                    tb->changed = true;

                    TraceLog(LOG_DEBUG,"left %d", tb->posB);
                }
                break;
            case KEY_RIGHT:
                if (tb->posB < (tb->lenB - 1)) {
                    if (tb->posA >= tb->lenA) if (!realloc_bufA(tb)) return; 
                    TraceLog(LOG_DEBUG,"right"); 
                    tb->bufA[tb->posA] = tb->bufB[tb->posB + 1];
                    tb->posB++;
                    tb->posA++;
                    tb->changed = true;

                }
                break;
            case KEY_ENTER:
                if (tb->posA >= tb->lenA) if (!realloc_bufA(tb)) return; 
                tb->bufA[tb->posA] = '\n';
                tb->posA += 1;
                tb->changed = true;

                break;
            default:
                TraceLog(LOG_DEBUG,"your car"); 
                return;
            }
        }
}

void layout_textbox(textbox * tb){
  char * text = malloc(tb->posA + (tb->lenB - tb->posB));
  memcpy(text,tb->bufA,tb->posA);
  memcpy(text+tb->posA,tb->bufB + tb->posB + 1, tb->lenB - tb->posB - 1);
  Clay_String str = (Clay_String){.isStaticallyAllocated = false, .length = tb->posA + (tb->lenB - tb->posB) - 1, .chars = text};
  CLAY_AUTO_ID({ .layout = { .sizing = { .width = CLAY_SIZING_FIXED(400), .height = CLAY_SIZING_FIXED(400)} }
                                  , .backgroundColor = COL_LIGHT
                                  , .clip = { .horizontal = true, .vertical = true, .childOffset = Clay_GetScrollOffset() }}) {
          CLAY_TEXT( str , CLAY_TEXT_CONFIG({ .fontSize = 16, .fontId = 0, .textColor = {0,0,0,255} }));
  }
}
#endif
