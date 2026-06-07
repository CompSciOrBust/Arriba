#pragma once

#include <functional>
#include <arribaPrimitives.h>

namespace Arriba::Elements {
    class Button : public Arriba::Primitives::Quad {
        private:
            Arriba::Primitives::Text* text;
            std::vector<std::function<void()>> callbacks;

        public:
            Button();

            void onFrame() override;
            void setText(const char* text);
            void setText(const char32_t* text);
            void registerCallback(std::function<void()> func);
    };
}  //namespace Arriba::Elements
