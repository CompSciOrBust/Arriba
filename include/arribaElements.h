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
            glm::vec4 neutral = {0.22,0.47,0.93,1};
            glm::vec4 highlightA = {0.1,0.95,0.98,1};
            glm::vec4 highlightB = {0.5,0.85,1,1};
            glm::vec4 activatedColour = {1,1,1,1};

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
            Arriba::Primitives::Quad* root = nullptr;
            float inertia = 0;
            int itemHeight = 70;
            int itemCount = 0;
            int selectedIndex = -1;
            int lastSelectedIndex = -1;
            glm::vec4 neutral = {0.22,0.47,0.93,1};
            glm::vec4 highlightA = {0.1,0.95,0.98,1};
            glm::vec4 highlightB = {0.5,0.85,1,1};
            glm::vec4 activatedColour = {1,1,1,1};

        public:
            InertialList(int, int, int, int, std::vector<std::string>);
            ~InertialList();
            void updateStrings(std::vector<std::string>);
            virtual void onFrame();
            virtual void update();
            void registerCallback(void (*)(int));
    };
}