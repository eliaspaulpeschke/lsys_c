#include <string.h>
#include "../clay/clay.h"
#include "common.h"
#include "inputbox.h"

inputbox * mk_inputbox(unsigned int max_len /*default 64*/){
    inputbox * ipb = malloc(sizeof(inputbox));
    if (max_len == 0) max_len = 64;
    if (!ipb) return NULL;
    *ipb = (inputbox){ .text = malloc(max_len)
                     , .max_len = max_len
                     , .cursor = 0
                     , .clay_id_num = NUM_INPUT_IDS + NAMESPACE_INPUTBOX 
                     , .changed = true
                     };
    if (!ipb->text) {
        free(ipb);
        return NULL;
    }
    NUM_INPUT_IDS += 1;
    memset(ipb->text, '\0', ipb->max_len);
    return ipb;
}

void free_inputbox(inputbox * ipb){
    free(ipb->text);
}

bool update_inputbox(inputbox * ipb){
    bool ptr = Clay_PointerOver(Clay_GetElementIdWithIndex(CLAY_STRING("inputbox"), ipb->clay_id_num));
    if (!ptr) return false;
    char chr = GetCharPressed();
    bool ctrl = IsKeyDown(KEY_LEFT_CONTROL) 
             || IsKeyDown(KEY_RIGHT_CONTROL);
    KeyboardKey key = GetKeyPressed();
    if (chr != 0 && !ctrl){
        if (ipb->cursor >= ipb->max_len -1) return true;
        if (ipb->text[ipb->cursor] != '\0') {
            unsigned int movelen = strlen(ipb->text + ipb->cursor);
            if (ipb->cursor + movelen + 1 >= ipb->max_len) return true;
            memmove(ipb->text + ipb->cursor + 1, ipb->text + ipb->cursor, movelen);
        }
        ipb->text[ipb->cursor] = chr;
        ipb->cursor++;
        ipb->changed = true;
        return true;
    } else if (key != KEY_NULL) {
        switch (key) {
            case KEY_BACKSPACE:
                if (ipb->cursor <= 0) return true;
                ipb->cursor--;
                ipb->text[ipb->cursor] = '\0';
                if (ipb->text[ipb->cursor+1] != '\0') {
                    unsigned int movelen = strlen(ipb->text + ipb->cursor + 1);
                    memmove( ipb->text + ipb->cursor
                           , ipb->text + ipb->cursor+1
                           , movelen);
                    ipb->text[ipb->cursor + movelen] = '\0';
                }
                return true;
            case KEY_LEFT:
                if (ipb->cursor > 0) ipb->cursor--;
                return true;
            case KEY_RIGHT:
                if (ipb->text[ipb->cursor] == '\0') return true;
                if (ipb->cursor < ipb->max_len -1) ipb->cursor++;
                return true;
            default: 
                break;
        }
    }
    return false;
}

void layout_inputbox(inputbox * ipb, Font * font, bool focus){
    Clay_String text = (Clay_String){ .isStaticallyAllocated = false, .length = strlen(ipb->text), .chars = ipb->text};
    Clay_BorderElementConfig border_focused = (Clay_BorderElementConfig){.width = {2,2,2,2,0}, .color = COL_ACCENT};
    Clay_BorderElementConfig border_normal = (Clay_BorderElementConfig){.width = {1,1,1,1,0}, .color = COL_DARK};
    Vector2 cursorPos = get_cursor_offset(ipb->text, ipb->cursor, font, 16,0, 1.0);
    CLAY(CLAY_IDI("inputbox", ipb->clay_id_num), { .layout = { .sizing = {CLAY_SIZING_FIXED(128), CLAY_SIZING_FIXED(24)} 
                                                             , .childAlignment = {CLAY_ALIGN_X_LEFT, CLAY_ALIGN_Y_CENTER}
                                                             , .padding = CLAY_PADDING_ALL(4) }
                                                 , .cornerRadius = CLAY_CORNER_RADIUS(8)
                                                 , .backgroundColor = COL_LIGHT
                                                 , .border = focus ? border_focused : border_normal
                                                 , .clip = {.horizontal=true, .vertical=true, .childOffset=Clay_GetScrollOffset() }}){
        Clay_Vector2 scr = Clay_GetScrollOffset();
        CLAY_AUTO_ID({.floating = { .offset = (Clay_Vector2){cursorPos.x + 4 + scr.x, cursorPos.y - 2+ scr.y}
                                    , .attachTo = CLAY_ATTACH_TO_PARENT
                                    , .expand = { .width=1, .height=8 }
                                    , .clipTo = CLAY_CLIP_TO_ATTACHED_PARENT }
                      , .backgroundColor = focus ? COL_ACCENT : COL_TRANSPARENT }){};

        CLAY_TEXT(text, CLAY_TEXT_CONFIG({ .fontSize = 16, .fontId = 0, .textColor = {0,0,0,255}, .lineHeight = 16.0 }));
    };
}
