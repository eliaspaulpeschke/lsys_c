#include "fonts.h"
#include <stdlib.h>

static Font * inner_fonts = NULL;

Font * get_fonts(){
    inner_fonts = malloc(sizeof(Font) * FONT_NUM);
    inner_fonts[0] = LoadFontEx("resources/fonts/roboto_mono/static/RobotoMono-Bold.ttf",48,NULL,0);
    SetTextureFilter(inner_fonts[0].texture, TEXTURE_FILTER_BILINEAR);
    return inner_fonts;
}

void free_fonts(){
    free(inner_fonts);
}
