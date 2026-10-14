#ifndef __SWITCH__
#include <font/FontBackendDesktop.h>
#include <arribaFont.h>
#include <stdio.h>

namespace Arriba::Font {

void FontBackendDesktop::open(int size) {
    if (FT_Init_FreeType(&ft)) printf("Failed to init free type\n");
    FT_New_Face(ft, Arriba::Font::fontPath.c_str(), 0, &face);
    FT_Set_Pixel_Sizes(face, 0, size);
}

void FontBackendDesktop::close() {
    FT_Done_Face(face);
    FT_Done_FreeType(ft);
}

}  // namespace Arriba::Font
#endif
