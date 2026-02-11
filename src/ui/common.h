#ifndef COMMON_H
#define COMMON_H
#include "../clay/clay.h"
#include <raylib.h>
#include "string.h"
 
#define NAMESPACE_SIZE 999 
#define NAMESPACE_TEXTBOX 1000
#define NAMESPACE_INPUTBOX 2000
#define NAMESPACE_MODULE 3000
#define NAMESPACE_MOVE_CONT 4000

#define COL_LIGHT         (Clay_Color) {220, 220, 230, 255}
#define COL_ACCENT        (Clay_Color) {90, 100, 230, 255}
#define COL_DARK          (Clay_Color) {80, 80, 80, 255}
#define COL_TRANSPARENT   (Clay_Color) {0, 0, 0, 0}

static inline Vector2 get_cursor_offset(char * text, unsigned int len, Font *font, float font_size, float letter_spacing, float line_height) {
    // Measure string size for Font
    Vector2 textSize = { 0 };

    float lineTextWidth = 0;
    int lineCharCount = 0;

    float textHeight = font_size;
    float scaleFactor = font_size/(float)font->baseSize;

    for (int i = 0; i < len; ++i, lineCharCount++)
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

static inline void add_label(char * label){
        Clay_String str = (Clay_String) { .isStaticallyAllocated = false, .length = strlen(label), .chars = label }; 
        CLAY_TEXT(str, CLAY_TEXT_CONFIG({ .fontSize = 16, .fontId = 0, .textColor = {0,0,0,255}, .lineHeight = 16.0 }));
}

#endif
