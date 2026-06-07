#pragma once

#include <functional>
#include <arribaPrimitives.h>
#include <arribaMaths.h>
#include <vector>

namespace Arriba::Elements {

    class InertialList : public Arriba::Graphics::AdvancedTexture, public Arriba::Primitives::Quad {
        private:
            std::vector<std::function<void(int)>> callbacks;
            std::vector<std::function<void(int, Arriba::Maths::vec2<float>)>> altCallbacks;
            Arriba::Primitives::Quad* root = nullptr;
            Arriba::Primitives::Quad* bg = nullptr;
            float inertia = 0;
            int itemHeight = 70;
            int itemCount = 0;
            int selectedIndex = -1;
            int lastSelectedIndex = -1;
            float stickMovementAccumulator = 0.0;

        public:
            InertialList(int x, int y, int width, int height, const std::vector<std::string>& strings);
            InertialList(int x, int y, int width, int height, const std::vector<std::u32string>& strings);
            ~InertialList();
            void updateStrings(const std::vector<std::string>& strings);
            void updateStrings(const std::vector<std::u32string>& strings);
            void onFrame() override;
            void update() override;
            void registerCallback(std::function<void(int)> func);
            void registerAltCallback(std::function<void(int, Arriba::Maths::vec2<float>)> func);
    };
} // namespace Arriba::Elements
