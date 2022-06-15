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

    class Text : public Arriba::Graphics::AdvancedTexture, public Quad
    {
        protected:
            int fontSize;
            Arriba::Maths::vec4<float> fontColour = {0,0,0,1};
            //Keep track of characters for memory management
            std::vector<Arriba::UIObject *> chars;

        public:
            Text(const char*, int);
            void setText(const char*);
            virtual void setColour(Arriba::Maths::vec4<float>);
            virtual void update();
    };
}