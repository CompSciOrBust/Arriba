#pragma once

#include <font/FontBackend.h>
#include <switch.h>

namespace Arriba::Font {

class FontBackendHOS : public FontBackend {
public:
    void open(int size) override;
    void tryFallbackGlyph(FT_UInt& glyphIndex, char32_t c, int size) override;
    void close() override;

private:
    bool openFallback(FT_Face& fallback, PlSharedFontType type);
};

}  // namespace Arriba::Font
