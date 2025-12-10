#pragma once
#include <arriba.h>
#include <arribaPrimitives.h>
#include <arribaGraphics.h>
#include <vector>

namespace Arriba::Elements {
class Button : public Arriba::Primitives::Quad {
    private:
        Arriba::Primitives::Text* text;
        std::vector<void (*)()> callbacks;

    public:
        Button();

        virtual void onFrame();
        void setText(const char* _text);
        void setText(const char32_t* _text);
        void registerCallback(void (*func)());
};

class InertialList : public Arriba::Graphics::AdvancedTexture, public Arriba::Primitives::Quad {
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
        InertialList(int _x, int _y, int _width, int _height, std::vector<std::string> strings);
        InertialList(int _x, int _y, int _width, int _height, std::vector<std::u32string> strings);
        ~InertialList();
        void updateStrings(std::vector<std::string> strings);
        void updateStrings(std::vector<std::u32string> strings);
        virtual void onFrame();
        virtual void update();
        void registerCallback(void (*func)(int));
        void registerAltCallback(void (*func)(int, Arriba::Maths::vec2<float>));
};
}  // namespace Arriba::Elements
