#ifndef COMMON_H
#define COMMON_H
#include "../clay.h"
#include <raylib.h>
#include "string.h"


const Clay_Color COL_LIGHT = (Clay_Color) {220, 220, 230, 255};
const Clay_Color COL_ACCENT = (Clay_Color) {90, 100, 230, 255};
const Clay_Color COL_DARK = (Clay_Color) {80, 80, 80, 255};
const Clay_Color COL_TRANSPARENT = (Clay_Color) {0, 0, 0, 0};

Vector2 get_cursor_offset(char * text, uint len, Font *font, float font_size, float letter_spacing, float line_height) {
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

#endif
