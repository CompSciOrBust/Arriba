#pragma once
#include <arriba.h>

namespace Arriba::Primitives {
        class Quad : public Arriba::UIObject {
        protected:
        float left, right, top, bottom;
        
        public:
        int width, height;
        Quad(int _x, int _y, int _width, int _height, Arriba::Graphics::Pivot pivotMode);
        
        void setDimensions(int _width, int _height, Arriba::Graphics::Pivot pivotMode);
        float getTop();
        float getBottom();
        float getLeft();
        float getRight();
    };
}  // namespace Arriba::Primitives
