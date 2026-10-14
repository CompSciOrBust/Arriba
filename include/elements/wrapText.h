#pragma once

#include <arribaPrimitives.h>
#include <string>

namespace Arriba::Elements {
    class WrapText : public Arriba::Primitives::Text {
    protected:
        std::u32string storedText;
        int wrapWidth;

    public:
        bool centreJustified = true;
        WrapText(const char* text, int size, int wrapWidth);
        WrapText(const char32_t* text, int size, int wrapWidth);
        void setText(const char* text);
        void setText(const char32_t* text);
        void setWrapWidth(int width);
    };
}  // namespace Arriba::Elements
