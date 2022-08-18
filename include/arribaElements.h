#pragma once
#include <arriba.h>
#include <arribaPrimitives.h>
#include <arribaGraphics.h>
#include <vector>

namespace Arriba::Elements
{
    class Button : public Arriba::Primitives::Quad
    {
        private:
            Arriba::Primitives::Text* text;
            std::vector<void (*)()> callbacks;

        public:
            Button();

            virtual void onFrame();
            void setText(const char*);
            void registerCallback(void (*)());
    };

    class InertialList : public Arriba::Graphics::AdvancedTexture, public Arriba::Primitives::Quad
    {
        private:
            std::vector<void (*)(int)> callbacks;
            std::vector<void (*)(int, Arriba::Maths::vec2<float>)> altCallbacks;
            Arriba::Primitives::Quad* root = nullptr;
            Arriba::Primitives::Quad* bg = nullptr;
            float inertia = 0;
            int itemHeight = 70;
            int itemCount = 0;
            int selectedIndex = -1;
            int lastSelectedIndex = -1;
            float stickMovementAcumulator = 0.0;

        public:
            InertialList(int, int, int, int, std::vector<std::string>);
            ~InertialList();
            void updateStrings(std::vector<std::string>);
            virtual void onFrame();
            virtual void update();
            void registerCallback(void (*)(int));
            void registerAltCallback(void (*)(int, Arriba::Maths::vec2<float>));
    };
}