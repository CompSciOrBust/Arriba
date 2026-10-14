#ifdef __SWITCH__
#include <font/FontBackendHOS.h>

namespace Arriba::Font {

void FontBackendHOS::open(int size) {
    if (FT_Init_FreeType(&ft)) printf("Failed to init free type\n");
    plInitialize(PlServiceType_User);
    PlFontData standardFontData;
    plGetSharedFontByType(&standardFontData, PlSharedFontType_Standard);
    FT_New_Memory_Face(ft, reinterpret_cast<FT_Byte*>(standardFontData.address), standardFontData.size, 0, &face);
    FT_Set_Pixel_Sizes(face, 0, size);
}

bool FontBackendHOS::openFallback(FT_Face& fallback, PlSharedFontType type) {
    PlFontData fontData;
    if (R_FAILED(plGetSharedFontByType(&fontData, type))) return false;
    return FT_New_Memory_Face(ft, reinterpret_cast<FT_Byte*>(fontData.address), fontData.size, 0, &fallback) == 0;
}

void FontBackendHOS::tryFallbackGlyph(FT_UInt& glyphIndex, char32_t c, int size) {
    if (glyphIndex != 0) return;
    static const PlSharedFontType fallbacks[] = {
        PlSharedFontType_ChineseTraditional,
        PlSharedFontType_ChineseSimplified,
    };
    for (PlSharedFontType type : fallbacks) {
        FT_Face fallback;
        if (openFallback(fallback, type)) {
            FT_UInt idx = FT_Get_Char_Index(fallback, c);
            if (idx != 0) {
                FT_Done_Face(face);
                face = fallback;
                glyphIndex = idx;
                FT_Set_Pixel_Sizes(face, 0, size);
                return;
            }
            FT_Done_Face(fallback);
        }
    }
}

void FontBackendHOS::close() {
    FT_Done_Face(face);
    FT_Done_FreeType(ft);
    plExit();
}

}  // namespace Arriba::Font
#endif
