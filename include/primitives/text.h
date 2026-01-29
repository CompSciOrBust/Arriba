#pragma once
#include <primitives/quad.h>
#include <arribaMaths.h>
#include <vector>

namespace Arriba::Primitives {
    class Text : public Arriba::Graphics::AdvancedTexture, public Quad {
        protected:
            int fontSize;
            Arriba::Maths::vec4<float> fontColour = {0, 0, 0, 1};
            // Keep track of characters for memory management
            std::vector<Arriba::UIObject *> chars;

        public:
            Text(const char*, int);
            Text(const char32_t*, int);
            void setText(const char* text);
            void setText(const char32_t* text);
            virtual void setColour(Arriba::Maths::vec4<float> _colour);
            virtual void update();
    };
}  // namespace Arriba::Primitives
