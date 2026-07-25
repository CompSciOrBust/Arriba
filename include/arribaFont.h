#pragma once

#include <ft2build.h>
#include FT_FREETYPE_H
#ifdef __SWITCH__
#include <switch.h>
#endif

namespace Arriba::Font {

#ifndef __SWITCH__
inline std::string fontPath;
#endif

inline void loadFont(FT_Library ft, FT_Face& face) {
    #ifdef __SWITCH__
    plInitialize(PlServiceType_User);
    PlFontData standardFontData;
    plGetSharedFontByType(&standardFontData, PlSharedFontType_Standard);
    FT_New_Memory_Face(ft, reinterpret_cast<FT_Byte*>(standardFontData.address), standardFontData.size, 0, &face);
    #else
    FT_New_Face(ft, fontPath.c_str(), 0, &face);
    #endif
}

inline void unloadFont(FT_Library ft, FT_Face& face) {
    FT_Done_Face(face);
    FT_Done_FreeType(ft);
    #ifdef __SWITCH__
    plExit();
    #endif
}

}  // namespace Arriba::Font
