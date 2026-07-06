#pragma once

#include <functional>
#include <arribaPrimitives.h>
#include <arribaMaths.h>
#include <vector>

namespace Arriba::Elements {

    class InertialList : public Arriba::Primitives::Quad {
        private:
            std::vector<std::function<void(int)>> callbacks;
            std::vector<std::function<void(int, Arriba::Maths::vec2<float>)>> altCallbacks;
            Arriba::Primitives::Quad* root = nullptr;
            Arriba::Primitives::Quad* bg = nullptr;
            std::unique_ptr<Arriba::Graphics::AdvancedTexture> texture = nullptr;
            float inertia = 0;
            int itemHeight = 70;
            int itemCount = 0;
            int selectedIndex = -1;
            int lastSelectedIndex = -1;
            float stickMovementAccumulator = 0.0;
            std::vector<std::u32string> cachedStrings;
            void spawnTextForItem(unsigned int index);
            void handleControllerInput();
            void handleTouchInput();
            void updateItemColours(float fadeTime);
            void updateFrameBuffer();

        public:
            InertialList(int x, int y, int width, int height, const std::vector<std::string>& strings);
            InertialList(int x, int y, int width, int height, const std::vector<std::u32string>& strings);
            ~InertialList();
            void updateStrings(const std::vector<std::string>& strings);
            void updateStrings(const std::vector<std::u32string>& strings);
            void onFrame() override;
            void registerCallback(std::function<void(int)> func);
            void registerAltCallback(std::function<void(int, Arriba::Maths::vec2<float>)> func);
    };
} // namespace Arriba::Elements
