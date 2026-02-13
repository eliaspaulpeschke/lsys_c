#include <stdbool.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <stdlib.h>
#include <raylib.h>
#include <raymath.h>
#include "../clay/clay.h"
#include "common.h"
#include "textbox.h"

static unsigned int NUM_TEXTBOX_IDS = 0;

Textbox mk_textbox(unsigned int max_len){
    char * text = malloc(2048);
    if (!text) return ERR_TEXTBOX;
    char * a = malloc(1024);
    if (!a) { free(text); return ERR_TEXTBOX; }
    char * b = malloc(1024);
    if (!b) { free(text); free(b); return ERR_TEXTBOX; }
    Textbox tb = (Textbox){ .text = text
                       , .changed = false
                       , .lenText = 2048
                       , .bufA = a 
                       , .bufB = b
                       , .lenA = 1024
                       , .lenB = 1024
                       , .posA = 0
                       , .posB = 1023
                       , .max_len = max_len
                       , .clay_id_num = NUM_TEXTBOX_IDS 
                           + NAMESPACE_TEXTBOX 
                       };
    memset(tb.bufA, '\0', tb.lenA);
    memset(tb.bufB, '\0', tb.lenB);
    memset(tb.text, '\0', tb.lenText);
    NUM_TEXTBOX_IDS += 1;
    return tb;
}

void free_textbox(Textbox tb){
    free(tb.bufA);
    free(tb.bufB);
    free(tb.text);
}

bool realloc_bufA(Textbox * tb, unsigned int extra){
  unsigned int newlen = tb->lenA << 1;
  if (newlen < (tb->lenA + extra)) newlen += extra;  
  if (newlen + tb->lenB >= tb->max_len) return false;
  char * temp = realloc(tb->bufA, newlen);
  bool success = false;
  if (temp != NULL) {
      memset(tb->bufA + tb->lenA, '\0', tb->lenA);
      tb->bufA = temp;
      tb->lenA = newlen;
      success = true;
  } 
  TraceLog(LOG_DEBUG, "realloc a %b", success);
  return success;
} 

bool realloc_bufB(Textbox * tb, unsigned int extra){
  unsigned int newlen = tb->lenB << 1;
  if (newlen < (tb->lenB + extra)) newlen += extra;  
  TraceLog(LOG_DEBUG, "%u %u %u", tb->lenB, newlen, extra); 
  if (newlen + tb->lenA >= tb->max_len) return false;
  char * temp = malloc(newlen);
  bool success = false;
  if (temp != NULL) {
     unsigned int pos = newlen - (tb->lenB - tb->posB);
     memset(temp, '\0', newlen);
     memcpy(temp + pos, tb->bufB+tb->posB, tb->lenB - tb->posB);
     free(tb->bufB);
     tb->bufB = temp;
     tb->posB = pos;
     tb->lenB = newlen;
     success = true;
  }
  TraceLog(LOG_DEBUG, "realloc b %b", success);
  return success;
}

void print_tb(Textbox * tb){
    unsigned int lb = tb->lenB - tb->posB;
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

void textbox_update_text(Textbox * tb){
    int len = tb->posA + (tb->lenB - tb->posB);
    tb->lenText = len;
    if (tb->changed){
        if (tb->lenTextBuf < len - 2) {
            char * temp = realloc(tb->text, len * 2);
            if (temp == NULL) {
                return;
            }
            tb->text = temp;
            tb->lenTextBuf = len * 2;
        }

        memcpy(tb->text,tb->bufA,tb->posA);
        memcpy(tb->text+tb->posA,tb->bufB + tb->posB + 1, tb->lenB - tb->posB - 1);
        tb->text[len - 1] = '\0';
    }
}

void textbox_paste(Textbox * tb){
    const char * clip = GetClipboardText();
    if (clip == NULL) return;
    unsigned int len = strlen(clip);
    TraceLog(LOG_DEBUG, "paste \"%s\": %d", clip, len); 
    if (tb->lenA - tb->posA - 1< len) if (!realloc_bufA(tb, len)) return;
    memcpy(tb->bufA + tb->posA, clip, len);
    tb->posA += len;
    tb->bufA[tb->posA] = '\0';
    tb->changed = true;
}

void textbox_copy(Textbox * tb){
    if (tb->changed) textbox_update_text(tb);
    SetClipboardText(tb->text);
}

bool update_textbox(Textbox * tb, bool focused_anyway){
    bool ptr = Clay_PointerOver(Clay_GetElementIdWithIndex(CLAY_STRING("textbox"), tb->clay_id_num));

    if (!ptr && !focused_anyway) return false;

    char chr = GetCharPressed();
    bool ctrl = IsKeyDown(KEY_LEFT_CONTROL) || IsKeyDown(KEY_RIGHT_CONTROL);
    KeyboardKey key = GetKeyPressed();
    if (chr != 0 && !ctrl){
        if (tb->posA >= tb->lenA) if (!realloc_bufA(tb, 1)) return true; 
        tb->bufA[tb->posA] = chr;
        tb->posA += 1;
        tb->changed = true;
    } else if (key != KEY_NULL) {
        TraceLog(LOG_DEBUG,"keying"); 
        int len_til_newline;
        int len_til_next_newline;
        unsigned int dist;
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
                    if (tb->posB == 0) if (!realloc_bufB(tb, 1)) return true;
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
                    if (tb->posA >= tb->lenA) if (!realloc_bufA(tb, 1)) return true; 
                    TraceLog(LOG_DEBUG,"right"); 
                    tb->bufA[tb->posA] = tb->bufB[tb->posB + 1];
                    tb->posB++;
                    tb->posA++;
                    tb->changed = true;
                }
                break; 
             case KEY_UP: ;
                len_til_newline = -1;
                len_til_next_newline = -1;
                for (int i = tb->posA - 1; i >= 0; i--){
                    if (tb->bufA[i] == '\n') {
                        if (len_til_newline < 0){
                          len_til_newline = tb->posA - i;
                        } else if (len_til_next_newline < 0){
                          len_til_next_newline = (tb->posA - i) - len_til_newline;
                          break;
                        } 
                    }
                }
                if (len_til_newline < 0) return true;
                if (len_til_next_newline < 0) len_til_next_newline = tb->posA - len_til_newline + 1;
                dist = len_til_newline >= len_til_next_newline ? len_til_newline : len_til_next_newline;
                if (tb->posB < dist) if (!realloc_bufB(tb, dist)) return true;
                //the -1 and +1 probably make more sense somewhere above but I have a fever and cannot figure it out
                memcpy(tb->bufB + (tb->posB - dist), tb->bufA + (tb->posA - dist)-1, dist+1); 
                memset(tb->bufA + (tb->posA - dist), '\0', dist);
                tb->posA -= dist;
                tb->posB -= dist;
                tb->changed = true;
                TraceLog(LOG_DEBUG, "down"); 
                break;
            case KEY_DOWN: ;
                unsigned int len_til_prev_newline = -1;
                for (int i = tb->posA-1; i >= 0; i--){
                    if (tb->bufA[i] == '\n') {
                        len_til_prev_newline = tb->posA - i;
                        break;
                    }
                }
                if (len_til_prev_newline == -1) len_til_prev_newline = tb->posA + 1;
                len_til_newline = -1;
                len_til_next_newline = -1;
                for (int i = tb->posB + 1; i < tb->lenB; i++){
                    if (tb->bufB[i] == '\n') {
                        if (len_til_newline < 0){
                          len_til_newline = i - tb->posB;
                        } else if (len_til_next_newline < 0){
                          len_til_next_newline = (i - tb->posB) - len_til_newline;
                          break;
                        } 
                    }
                } 
                unsigned int line_len = len_til_prev_newline + len_til_newline;
                if (len_til_newline < 0) return true;
                if (len_til_next_newline < 0) len_til_next_newline = (tb->lenB - tb->posB) - len_til_newline;
                dist = len_til_prev_newline > len_til_next_newline ? len_til_newline + len_til_next_newline -1 : line_len -1;
                TraceLog(LOG_DEBUG, "dist %u %d %d", dist, len_til_newline, len_til_next_newline); 
                if ((tb->lenA - tb->posA) < dist) if (!realloc_bufA(tb, dist)) return true;
                //the -1 and +1 probably make more sense somewhere above but I have a fever and cannot figure it out
                memcpy(tb->bufA + tb->posA, tb->bufB + tb->posB + 1, dist); 
                memset(tb->bufB + tb->posB + 1, '\0', dist);
                tb->posA += dist;
                tb->posB += dist;
                tb->changed = true;
                TraceLog(LOG_DEBUG, "check me!!! up!"); 
                break;

            case KEY_ENTER:
                if (tb->posA >= tb->lenA) if (!realloc_bufA(tb, 1)) return true; 
                tb->bufA[tb->posA] = '\n';
                tb->posA += 1;
                tb->changed = true;

                break;
            case KEY_C:
                if (ctrl) {
                    textbox_copy(tb);
                    tb->changed = true;
                }
                break;
            case KEY_V:
                if (ctrl) {
                    textbox_paste(tb);
                    tb->changed = true;
                }
                break;
            default:
                TraceLog(LOG_DEBUG,"your car"); 
                return false;
            }
        }
    textbox_update_text(tb);
    return true;
}

void layout_textbox(Textbox tb, Font * font){
  Clay_String str = (Clay_String){.isStaticallyAllocated = false, .length = tb.lenText - 1, .chars = tb.text};
  Vector2 cursorPos = get_cursor_offset(tb.bufA, strlen(tb.bufA), font, 16,0, 1.0);

  CLAY(CLAY_IDI("textbox", tb.clay_id_num), { .layout = { .layoutDirection = CLAY_LEFT_TO_RIGHT
                                                         , .padding = CLAY_PADDING_ALL(8) 
                                                         , .sizing = {CLAY_SIZING_GROW(0), CLAY_SIZING_GROW(0)} }
                                             , .clip = {.horizontal = true, .vertical = true, .childOffset = Clay_GetScrollOffset()}
                                             , .backgroundColor = COL_LIGHT
                                             }) {

              Clay_Vector2 scr = Clay_GetScrollOffset();
                  
              CLAY_AUTO_ID({.floating = { .offset = (Clay_Vector2){cursorPos.x + 8 + scr.x, cursorPos.y + scr.y}
                                    , .attachTo = CLAY_ATTACH_TO_PARENT
                                    , .expand = { .width=1, .height=8 }
                                    , .clipTo = CLAY_CLIP_TO_ATTACHED_PARENT
                                    , .zIndex = 1}
                        , .backgroundColor = COL_ACCENT});
              CLAY_TEXT( str , CLAY_TEXT_CONFIG({ .fontSize = 16, .fontId = 0, .textColor = {0,0,0,255}, .lineHeight = 16.0 }));
          };
  }
