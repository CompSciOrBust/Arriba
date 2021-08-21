#pragma once
#include <arriba.h>
#include <arribaGraphics.h>

namespace Arriba::Primitives
{
    class Quad : public Arriba::UIObject
    {
        protected:
        float left, right, top, bottom;

        public:
        int width, height;
        Quad(int, int, int, int, Arriba::Graphics::Pivot);
        
        void setDimensions(int, int, Arriba::Graphics::Pivot);
        float getTop();
        float getBottom();
        float getLeft();
        float getRight();
    };

    class Character : public Quad
    {
        public:
        Character(Arriba::Graphics::CharInfo);
    };

    class Text : public Quad
    {
        protected:
        int fontSize;
        glm::vec4 fontColour = {0,0,0,1};
        public:
        Text(const char*, int);
        void setText(const char*);
        virtual void setColour(glm::vec4);
    };
}