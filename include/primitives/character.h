#pragma once
#include <primitives/quad.h>

namespace Arriba::Primitives {
    class Character : public Quad
    {
        public:
        Character(Arriba::Graphics::CharInfo);
    };
}  // namespace Arriba::Primitives
