#pragma once
#include <arriba.h>
#include <arribaPrimitives.h>
#include <vector>

namespace Arriba::Elements
{
    class Button : public Arriba::Primitives::Quad
    {
        private:
            Arriba::Primitives::Text* text;
            std::vector<void (*)()> callbacks;
            float time = 0;
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

    /*TODO: Add lists
    class InertialList : Arriba::Primitives::Quad
    {
        private:
            float scrollOffset = 0;
            float inertia = 0;
            float textHeight = 25;
            std::vector<void (*)(int)> callbacks;
            std::vector<std::string> texts;
        public:
            InertialList();

            void registerCallback(void (*)(int));
            void setTexts(std::vector<std::string>);
    };*/
}