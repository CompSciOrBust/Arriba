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

            virtual void onFrame();
            void setText(const char* _text);
            void setText(const char32_t* _text);
            void registerCallback(std::function<void()> func);
    };
}  //namespace Arriba::Elements
