#include <elements/inertialList.h>
#include <functional>
#include <arribaText.h>

namespace Arriba::Elements {
    static constexpr float kInertiaDamping = 0.85f;
    static constexpr float kInertiaSeriesLimit = 1.0f / (1.0f - kInertiaDamping);

    InertialList::InertialList(int x, int y, int width, int height, const std::vector<std::string>& strings) : InertialList(x, y, width, height, Arriba::Text::ASCIIToUnicodeList(strings)) {}

    InertialList::InertialList(int x, int y, int width, int height, const std::vector<std::u32string>& strings) : Arriba::Primitives::Quad(x, y, width, height, Arriba::Graphics::Pivot::topLeft) {
        texture = std::make_unique<Arriba::Graphics::AdvancedTexture>(width, height);
        renderer->setTexture(texture->texID);
        bg = new Arriba::Primitives::Quad(0, 0, Quad::width, Quad::height, Arriba::Graphics::Pivot::topLeft);
        bg->setColour(Arriba::Colour::neutral);
        bg->setFBOwner(texture.get());
        root = new Arriba::Primitives::Quad(0, 0, 0, 0, Arriba::Graphics::Pivot::centre);
        root->setColour({0.0f, 0.0f, 0.0f, 0.0f});
        updateStrings(strings);
        root->setFBOwner(texture.get());
    }

    InertialList::~InertialList() {
        root->destroy();
    }

    void InertialList::updateStrings(const std::vector<std::string>& strings) {
        updateStrings(Arriba::Text::ASCIIToUnicodeList(strings));
    }

    void InertialList::updateStrings(const std::vector<std::u32string>& strings) {
        root->transform.position.y = 0;

        for (Arriba::UIObject* child : root->getChildren()) {
            child->destroy();
        }

        itemCount = strings.size();

        cachedStrings = strings;

        for (unsigned int i = 0; i < itemCount; i++) {
            Arriba::Primitives::Quad* itemContainer = new Arriba::Primitives::Quad(0, i * itemHeight, Quad::width, itemHeight, Arriba::Graphics::Pivot::topLeft);
            itemContainer->setParent(root);
            itemContainer->setColour(Arriba::Colour::neutral);
        }

        if (itemCount > 0) {
            selectedIndex = 0;
            lastSelectedIndex = 0;
        }

        bg->setDimensions(Quad::width, Quad::height + itemCount * itemHeight, Arriba::Graphics::Pivot::topLeft);
        bg->setColour(Arriba::Colour::neutral);
    }

    void InertialList::onFrame() {
        handleControllerInput();
        handleTouchInput();

        // If selected index is off screen add inertia to bring it on screen
        if (selectedIndex >= 0) {
            if (selectedIndex * itemHeight + root->transform.position.y < 0) inertia = (selectedIndex * itemHeight + root->transform.position.y) / -kInertiaSeriesLimit;
            else if ((selectedIndex+1) * itemHeight + root->transform.position.y > Quad::height) inertia = ((selectedIndex+1) * itemHeight + root->transform.position.y - Quad::height) / -kInertiaSeriesLimit;
        }

        root->transform.position.y += inertia;
        inertia *= kInertiaDamping;

        if (root->transform.position.y + itemHeight * itemCount < Quad::height) root->transform.position.y = Quad::height - itemCount * itemHeight;
        if (root->transform.position.y > 0) root->transform.position.y = 0;

        float fadeTime = 3 * Arriba::deltaTime;
        updateItemColours(fadeTime);
        bg->setColour(Arriba::Maths::lerp(bg->getColour(), Arriba::Colour::neutral, fadeTime));
        lastSelectedIndex = selectedIndex;
        updateFrameBuffer();
    }

    void InertialList::handleControllerInput() {
        if (Arriba::highlightedObject != this) return;

        if (Arriba::Input::buttonDown(Arriba::Input::controllerButton::DPadUp)) {
            if (selectedIndex >= 0 || itemCount * itemHeight < Quad::height) {
                selectedIndex -= 1;
                if (selectedIndex < 0) selectedIndex = itemCount - 1;
            } else {
                // No item is currently selected but don't just jump to index 0
                selectedIndex = static_cast<int>(-root->transform.position.y / itemHeight);
            }
        }
        if (Arriba::Input::buttonDown(Arriba::Input::controllerButton::DPadDown)) {
            if (selectedIndex >= 0 || itemCount * itemHeight < Quad::height) {
                selectedIndex += 1;
                if (selectedIndex > itemCount-1) selectedIndex = 0;
            } else {
                // No item is currently selected so select item at bottom of list
                selectedIndex = static_cast<int>(-root->transform.position.y / itemHeight);
            }
        }

        if (Arriba::Input::buttonDown(Arriba::Input::controllerButton::AButtonSwitch) && selectedIndex != -1) {
            for (auto& cb : callbacks) cb(selectedIndex);
        }

        if (Arriba::Input::buttonDown(Arriba::Input::controllerButton::YButtonSwitch) && selectedIndex != -1) {
            spawnTextForItem(selectedIndex);
            auto* selectedItemText = static_cast<Arriba::Primitives::Quad*>(root->getChildren()[selectedIndex]->getChildren()[0]);
            float menuX = transform.position.x + (float)selectedItemText->getRight();
            float menuY = root->transform.position.y + (selectedIndex + 1) * itemHeight + itemHeight * 0.5f;
            for (auto& cb : altCallbacks) cb(selectedIndex, Arriba::Maths::vec2<float>{menuX, menuY});
        }

        if (std::abs(Arriba::Input::AnalogStickLeft.yPos + Arriba::Input::AnalogStickRight.yPos) > 0.1f) {
            stickMovementAccumulator += (Arriba::Input::AnalogStickLeft.yPos + Arriba::Input::AnalogStickRight.yPos) * Arriba::deltaTime;
            if (std::abs(stickMovementAccumulator) > 0.07) {
                if (stickMovementAccumulator > 0) selectedIndex -= 1;
                else selectedIndex += 1;
                stickMovementAccumulator = 0.0;
                if (selectedIndex < 0) selectedIndex = 0;
                if (selectedIndex > itemCount-1) selectedIndex = itemCount-1;
            }
        } else {
            stickMovementAccumulator = 0.0;
        }
    }

    void InertialList::handleTouchInput() {
        if (Arriba::highlightedObject == this && Arriba::Input::touch.end && selectedIndex != -1 && activeLayer == layer) {
            if (Arriba::Input::touch.downTime < 0.5f) {
                for (auto& cb : callbacks) cb(selectedIndex);
            } else {
                for (auto& cb : altCallbacks) cb(selectedIndex, Arriba::Input::touch.pos);
            }
        }

        if (!Arriba::Input::touchScreenPressed() || Arriba::activeLayer != layer) return;

        float touchX = Arriba::Input::touch.pos.x;
        float touchY = Arriba::Input::touch.pos.y;
        if (touchY >= getTop() || touchY <= getBottom() || touchX >= getRight() || touchX <= getLeft()) {
            if (Arriba::highlightedObject == this) highlightedObject = nullptr;
            return;
        }

        Arriba::highlightedObject = this;
        if (std::abs(Arriba::Input::touch.origin.y - touchY) > itemHeight / 2) {
            inertia = -Arriba::Input::touch.delta.y;
            selectedIndex = -1;
        } else {
            float localY = touchY - Quad::getBottom();
            int idx = static_cast<int>((localY - root->transform.position.y) / itemHeight);
            selectedIndex = (idx >= 0 && idx < static_cast<int>(itemCount)) ? idx : -1;
        }
    }

    void InertialList::updateItemColours(float fadeTime) {
        for (unsigned int i = 0; i < itemCount; i++) {
            Arriba::UIObject* item = root->getChildren()[i];
            
            if (i != selectedIndex) {
                item->setColour(Arriba::Maths::lerp(item->getColour(), Arriba::Colour::neutral, fadeTime));
                continue;
            }

            if (Arriba::highlightedObject != this) {
                item->setColour(Arriba::Maths::lerp(item->getColour(), Arriba::Colour::highlightB, fadeTime));
            } else if (selectedIndex != lastSelectedIndex || Arriba::Input::buttonDown(Arriba::Input::controllerButton::AButtonSwitch)) {
                item->setColour(Arriba::Colour::activatedColour);
            } else {
                float lerpValue = (sin(Arriba::time*4) + 1) / 2;
                Arriba::Maths::vec4 targetColour = Arriba::Maths::lerp(Arriba::Colour::highlightA, Arriba::Colour::highlightB, lerpValue);
                item->setColour(Arriba::Maths::lerp(item->getColour(), targetColour, fadeTime));
            }
        }
    }

    void InertialList::updateFrameBuffer() {
        glBindFramebuffer(GL_FRAMEBUFFER, texture->FBO);
        drawTextureObject(bg);

        unsigned int listItemRenderIndex = -root->transform.position.y / itemHeight;
        unsigned int listItemRenderCount = listItemRenderIndex + height / itemHeight + 1;
        if (listItemRenderCount > itemCount) listItemRenderCount = itemCount;
        for (unsigned int i = listItemRenderIndex; i < listItemRenderCount; i++) {
            spawnTextForItem(i);
            Arriba::UIObject* container = root->getChildren()[i];
            container->renderer->updateParentTransform(root->renderer->getTransformMatrix());
            drawTextureObject(container);
        }
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
    }

    void InertialList::spawnTextForItem(unsigned int index) {
        Arriba::UIObject* container = root->getChildren()[index];
        if (container->getChildren().empty()) {
            Arriba::Primitives::Text* itemText = new Arriba::Primitives::Text(cachedStrings[index].c_str(), 48);
            itemText->setColour({0.0f, 0.0f, 0.0f, 1.0f});
            itemText->setParent(container);
            itemText->transform.position.x += itemText->width / 2 + Quad::width * 0.05;
            itemText->transform.position.y += itemHeight / 2;
        }
    }

    void InertialList::registerCallback(std::function<void(int)> func) {
        callbacks.push_back(func);
    }

    void InertialList::registerAltCallback(std::function<void(int, Arriba::Maths::vec2<float>)> func) {
        altCallbacks.push_back(func);
    }
} // namespace Arriba::Elements
