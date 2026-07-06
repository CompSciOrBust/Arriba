#pragma once

#include <primitives/quad.h>
#include <arribaMaths.h>
#include <vector>

namespace Arriba::Primitives {
    class Text : public Quad {
        protected:
            int fontSize;
            Arriba::Maths::vec4<float> fontColour = {0, 0, 0, 1};
            std::unique_ptr<Arriba::Graphics::AdvancedTexture> texture = nullptr;
            void updateFrameBuffer(const std::vector<Arriba::UIObject*>& chars);

        public:
            Text(const char* text, int size);
            Text(const char32_t* text, int size);
            void setText(const char* text);
            void setText(const char32_t* text);
            void setColour(const Arriba::Maths::vec4<float>& colour) override;
    };
}  // namespace Arriba::Primitives
