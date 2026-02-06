#ifndef TEXTBOX_H
#define TEXTBOX_H
#include <stdbool.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <stdlib.h>
#include <raylib.h>
#include <raymath.h>
#include "../clay.h"
#include "colors.h"
#include "../lsystem.h"
#include "../lsystem_parser.h"
#include "../turtle.h"


typedef struct {
    char * text;
    bool changed;
    uint lenTextBuf;
    uint lenText;
    char * bufA;
    char * bufB;
    uint lenA;
    uint lenB;
    uint posA;
    uint posB; //used backwards, start at lenB-1
    uint max_len;
    Vector2 size;
    Vector2 pos;
    uint clay_id_num;
    Lsystem * lsystem;
    char * generated;
    Turtle * init_turtle;
} textbox;

void free_textbox(textbox * tb){
    free(tb->bufA);
    free(tb->bufB);
    free(tb->text);
    free_lsystem(*tb->lsystem);
    free(tb->lsystem);
    free(tb->generated);
    free(tb);
}

bool realloc_bufA(textbox * tb, uint extra){
  uint newlen = tb->lenA << 1;
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

bool realloc_bufB(textbox * tb, uint extra){
  uint newlen = tb->lenB << 1;
  if (newlen < (tb->lenB + extra)) newlen += extra;  
  TraceLog(LOG_DEBUG, "%u %u %u", tb->lenB, newlen, extra); 
  if (newlen + tb->lenA >= tb->max_len) return false;
  char * temp = malloc(newlen);
  bool success = false;
  if (temp != NULL) {
     uint pos = newlen - (tb->lenB - tb->posB);
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

void textbox_update_text(textbox * tb){
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

void textbox_update_lsystem(textbox * tb){
    textbox_update_text(tb);
    if (tb->lsystem != NULL){
        free_lsystem(*tb->lsystem);
        free(tb->lsystem);
    }
    Lsystem * lsys = malloc(sizeof(Lsystem));
    char * lsys_text = malloc(tb->lenText);
    strlcpy(lsys_text, tb->text, tb->lenText);
    *lsys = lsystem_from_string(lsys_text, true);
    if (lsys->nrules <= 0 || lsys == NULL) return;
    tb->lsystem = lsys;
    TraceLog(LOG_DEBUG, "%s", lsys->axiom); 
    for (int i = 0; i < lsys->nrules; i++){
       TraceLog(LOG_DEBUG, "%s", str_rule(lsys->ruleset + i, "   "));
    }
    if (lsys->axiom != NULL) {
        if (tb->generated != NULL) {
            free(tb->generated);
        }
        tb->generated = strdup(lsys->axiom);
    }
}

void textbox_generate(textbox * tb){
    if (tb->generated == NULL || tb->lsystem == NULL) return;
    tb->generated = applyRules(tb->lsystem->ruleset, tb->lsystem->nrules, tb->generated, true);
}

void textbox_paste(textbox * tb){
    const char * clip = GetClipboardText();
    if (clip == NULL) return;
    uint len = strlen(clip);
    TraceLog(LOG_DEBUG, "paste \"%s\": %d", clip, len); 
    if (tb->lenA - tb->posA - 1< len) if (!realloc_bufA(tb, len)) return;
    memcpy(tb->bufA + tb->posA, clip, len);
    tb->posA += len;
    tb->bufA[tb->posA] = '\0';
    tb->changed = true;
}

void textbox_copy(textbox * tb){
    if (tb->changed) textbox_update_text(tb);
    SetClipboardText(tb->text);
}

bool update_textbox(textbox * tb){
    bool ptr =Clay_PointerOver(Clay_GetElementIdWithIndex(CLAY_STRING("textbox"), tb->clay_id_num));
    bool sizer=Clay_PointerOver(Clay_GetElementIdWithIndex(CLAY_STRING("textbox-sizer"), tb->clay_id_num));
    bool button_lsys=Clay_PointerOver(Clay_GetElementIdWithIndex(CLAY_STRING("textbox-btn-lsys"), tb->clay_id_num));
    bool button_gen=Clay_PointerOver(Clay_GetElementIdWithIndex(CLAY_STRING("textbox-btn-gen"), tb->clay_id_num));
    
    if (!(ptr || sizer || button_lsys || button_gen)) return false;

    if (IsMouseButtonReleased(0)) {
        if (button_lsys) {
           textbox_update_lsystem(tb);
        } else if (button_gen) {
            textbox_generate(tb);
        }    
        return true;
    }
    if (IsMouseButtonDown(0)){
        Vector2 mouse = GetMouseDelta();
        if (sizer) {
            tb->size = Vector2Add(mouse, tb->size);
        } else {
            tb->pos = Vector2Add(mouse, tb->pos);
        }
        return true;
    }

    char chr = GetCharPressed();
    bool ctrl = IsKeyDown(KEY_LEFT_CONTROL) || IsKeyDown(KEY_RIGHT_CONTROL);
    KeyboardKey key = GetKeyPressed();
    if (chr != 0 && !ctrl){
        if (tb->posA >= tb->lenA) if (!realloc_bufA(tb, 1)) return true; 
        tb->bufA[tb->posA] = chr;
        tb->posA += 1;
        tb->changed = true;
        TraceLog(LOG_DEBUG,"charring %d", tb->posA); 
    } else if (key != KEY_NULL) {
        TraceLog(LOG_DEBUG,"keying"); 
        int len_til_newline;
        int len_til_next_newline;
        uint dist;
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
                uint len_til_prev_newline = -1;
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
                uint line_len = len_til_prev_newline + len_til_newline;
                // 1234\n123456\n1234\n123456
                // 12
                //
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
                    return true;
                }
                break;
            case KEY_V:
                if (ctrl) {
                    textbox_paste(tb);
                    return true;
                }
                break;
            default:
                TraceLog(LOG_DEBUG,"your car"); 
                return false;
            }
        }
    return true;
}

Vector2 get_cursor_offset(char * text, Font *font, float font_size, float letter_spacing, float line_height) {
    // Measure string size for Font
    Vector2 textSize = { 0 };

    float lineTextWidth = 0;
    int lineCharCount = 0;

    float textHeight = font_size;
    float scaleFactor = font_size/(float)font->baseSize;

    for (int i = 0; i < strlen(text); ++i, lineCharCount++)
    {
        if (text[i] == '\n') {
            textHeight += line_height * font_size;
            lineTextWidth = 0;
            lineCharCount = 0;
            continue;
        }
        int index = text[i] - 32;
        if (font->glyphs[index].advanceX != 0) lineTextWidth += font->glyphs[index].advanceX;
        else lineTextWidth += (font->recs[index].width + font->glyphs[index].offsetX);
    }

    textSize.x = lineTextWidth * scaleFactor + (lineCharCount * letter_spacing);
    textSize.y = textHeight;

    return textSize;
}

void layout_textbox(textbox * tb, Font * font, bool focused){
  textbox_update_text(tb);
  Clay_BorderElementConfig border_focused = (Clay_BorderElementConfig){.width = {2,2,2,2,0}, .color = COL_ACCENT};
  Clay_BorderElementConfig border_normal = (Clay_BorderElementConfig){.width = {0,0,0,0,0}, .color = {0,0,0,0}};
  Clay_String str = (Clay_String){.isStaticallyAllocated = false, .length = tb->lenText - 1, .chars = tb->text};
  Vector2 cursorPos = get_cursor_offset(tb->bufA, font, 16,0, 1.0);
  CLAY(CLAY_IDI("textbox", tb->clay_id_num), { .floating = { //.expand = { .width = tb->size.x, .height = tb->size.y}
                                             .offset = (Clay_Vector2){tb->pos.x, tb->pos.y}
                                            , .attachTo = CLAY_ATTACH_TO_PARENT
                                            , .clipTo = CLAY_CLIP_TO_ATTACHED_PARENT}
                              , .layout = { .layoutDirection = CLAY_LEFT_TO_RIGHT }
                              , .backgroundColor = COL_LIGHT
                              , .border = focused ? border_focused : border_normal 
                              , .cornerRadius = CLAY_CORNER_RADIUS(8)}) {
          CLAY(CLAY_IDI("textbox-sizer", tb->clay_id_num), {.floating = { .offset = (Clay_Vector2){tb->size.x, tb->size.y-16}
                                    , .attachTo = CLAY_ATTACH_TO_PARENT
                                    , .expand = { .width=32, .height=32 } }
                       , .backgroundColor = COL_TRANSPARENT
                       , .cornerRadius = CLAY_CORNER_RADIUS(8)}){
              CLAY_AUTO_ID({.layout={ .sizing = { .width = CLAY_SIZING_FIXED(8), .height = CLAY_SIZING_FIXED(8)}}
                           ,.backgroundColor = COL_DARK
                           ,.cornerRadius=8}){};
          };
          CLAY_AUTO_ID({ .layout = { .sizing = {.width = CLAY_SIZING_FIXED(24), .height = CLAY_SIZING_GROW(0)}
                                   , .layoutDirection = CLAY_TOP_TO_BOTTOM }
                       , .border = {.width = {0,1,0,0,1}, .color = COL_DARK}
                       , .backgroundColor = COL_TRANSPARENT}){
              CLAY(CLAY_IDI("textbox-btn-lsys", tb->clay_id_num), { .layout = { .sizing = { .width = CLAY_SIZING_FIXED(24), .height = CLAY_SIZING_FIXED(24)}
                                                            , .childAlignment = {.x = CLAY_ALIGN_X_CENTER, .y = CLAY_ALIGN_Y_CENTER} }
                                                , .backgroundColor = (Clay_Hovered() ? COL_ACCENT : COL_TRANSPARENT) 
                                                }){ CLAY_TEXT(CLAY_STRING(">"), CLAY_TEXT_CONFIG({ .fontSize = 16, .fontId = 0, .textColor = {0,0,0,255}, .lineHeight = 16.0 })); };
              CLAY(CLAY_IDI("textbox-btn-gen", tb->clay_id_num), { .layout = { .sizing = { .width = CLAY_SIZING_FIXED(24), .height = CLAY_SIZING_FIXED(24)}
                                                            , .childAlignment = {.x = CLAY_ALIGN_X_CENTER, .y = CLAY_ALIGN_Y_CENTER} }
                                                , .backgroundColor = (Clay_Hovered() ? COL_ACCENT : COL_TRANSPARENT) 
                                                }){ CLAY_TEXT(CLAY_STRING("gen"), CLAY_TEXT_CONFIG({ .fontSize = 16, .fontId = 0, .textColor = {0,0,0,255}, .lineHeight = 16.0 })); };
          };

          CLAY_AUTO_ID({.layout = { .sizing = { .width = CLAY_SIZING_FIXED(tb->size.x)
                                              , .height = CLAY_SIZING_FIXED(tb->size.y) }
                                  , .padding = CLAY_PADDING_ALL(16)}
                                  , .clip = { .horizontal = true, .vertical = true, .childOffset = Clay_GetScrollOffset()}} ) {
              Clay_Vector2 scr = Clay_GetScrollOffset();
                  
              CLAY_AUTO_ID({.floating = { .offset = (Clay_Vector2){cursorPos.x + 16 + scr.x, cursorPos.y +8+ scr.y}
                                    , .attachTo = CLAY_ATTACH_TO_PARENT
                                    , .expand = { .width=1, .height=8 }
                                    , .clipTo = CLAY_CLIP_TO_ATTACHED_PARENT }
                        , .backgroundColor = COL_ACCENT});
              CLAY_TEXT( str , CLAY_TEXT_CONFIG({ .fontSize = 16, .fontId = 0, .textColor = {0,0,0,255}, .lineHeight = 16.0 }));
          };
  }
}
#endif
