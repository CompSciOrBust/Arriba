#pragma once

#include <ft2build.h>
#include FT_FREETYPE_H

namespace Arriba::Font {

class FontBackend {
public:
    FT_Library ft;
    FT_Face face;

    virtual ~FontBackend() = default;
    virtual void open(int size) = 0;
    virtual void tryFallbackGlyph(FT_UInt& glyphIndex, char32_t c, int size) {}
    virtual void close() = 0;
};

}  // namespace Arriba::Font
