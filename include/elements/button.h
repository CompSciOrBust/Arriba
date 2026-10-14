#pragma once

#include <functional>
#include <arribaPrimitives.h>
#include <elements/wrapText.h>

namespace Arriba::Elements {
    class Button : public Arriba::Primitives::Quad {
        private:
            Arriba::Elements::WrapText * text;
            std::vector<std::function<void()>> callbacks;
            int prevWidth = 0;

        public:
            Button();

            void onFrame() override;
            void setText(const char* text);
            void setText(const char32_t* text);
            void registerCallback(std::function<void()> func);
    };
}  //namespace Arriba::Elements
