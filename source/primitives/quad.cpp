#include <primitives/quad.h>

namespace Arriba::Primitives {
    Quad::Quad(int x, int y, int width, int height, Arriba::Graphics::Pivot pivotMode) {
        this->transform.position.x = x;
        this->transform.position.y = y;
        if ((width != 0) && (height != 0)) setDimensions(width, height, pivotMode);
    }

    void Quad::setDimensions(int width, int height, Arriba::Graphics::Pivot pivotMode) {
        this->width = width;
        this->height = height;

        switch (pivotMode) {
            case Arriba::Graphics::centre:      left = -width/2; right = width/2; top = height/2;  bottom = -height/2; break;
            case Arriba::Graphics::topLeft:     left = 0;        right = width;   top = height;    bottom = 0;         break;
            case Arriba::Graphics::bottomLeft:  left = 0;        right = width;   top = 0;         bottom = -height;   break;
            case Arriba::Graphics::topRight:    left = -width;   right = 0;       top = height;    bottom = 0;         break;
            case Arriba::Graphics::bottomRight: left = -width;   right = 0;       top = 0;         bottom = -height;   break;
        }

        // Texture Y coordinates here are a bit hacked to work around inversion issue
        // float verts[20] = {
        //     right,  top,    0.0f, 1.0f, 0.0f,  // top right
        //     right,  bottom, 0.0f, 1.0f, 1.0f,  // bottom right
        //     left,   bottom, 0.0f, 0.0f, 1.0f,  // bottom left
        //     left,   top,    0.0f, 0.0f, 0.0f  // top left
        // };

        float verts[20] = {
            right,  top,    0.0f, 1.0f, 1.0f,  // top right
            right,  bottom, 0.0f, 1.0f, 0.0f,  // bottom right
            left,   bottom, 0.0f, 0.0f, 0.0f,  // bottom left
            left,   top,    0.0f, 0.0f, 1.0f  // top left
        };

        unsigned int indexes[6] = {
            0, 1, 3,  // First triangle
            1, 2, 3  // Second triangle
        };

        renderer->loadVerts(verts, sizeof(verts), indexes, sizeof(indexes));
    }

    float Quad::getTop() {
        return top + getGlobalPos().col4.y;
    }

    float Quad::getBottom() {
        return bottom + getGlobalPos().col4.y;
    }

    float Quad::getLeft() {
        return left + getGlobalPos().col4.x;
    }

    float Quad::getRight() {
        return right + getGlobalPos().col4.x;
    }
}  // namespace Arriba::Primitives
