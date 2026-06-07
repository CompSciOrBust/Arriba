#include <elements/inertialList.h>
#include <functional>
#include <arribaText.h>

namespace Arriba::Elements {
    InertialList::InertialList(int x, int y, int width, int height, const std::vector<std::string>& strings) : InertialList(x, y, width, height, Arriba::Text::ASCIIToUnicodeList(strings)) {}

    InertialList::InertialList(int x, int y, int width, int height, const std::vector<std::u32string>& strings) : Arriba::Graphics::AdvancedTexture(width, height), Arriba::Primitives::Quad(x, y, width, height, Arriba::Graphics::Pivot::topLeft) {
        renderer->setTexture(texID);
        bg = new Arriba::Primitives::Quad(0, 0, Quad::width, Quad::height, Arriba::Graphics::Pivot::topLeft);
        bg->setColour(Arriba::Colour::neutral);
        bg->setFBOwner(this);
        root = new Arriba::Primitives::Quad(0, 0, 0, 0, Arriba::Graphics::Pivot::centre);
        root->setColour({0.0f, 0.0f, 0.0f, 0.0f});
        updateStrings(strings);
        root->setFBOwner(this);
    }

    InertialList::~InertialList() {
        root->destroy();
    }

    void InertialList::updateStrings(const std::vector<std::string>& strings) {
        std::vector<std::u32string> _strings;
        for (const std::string& str : strings) {
            char32_t* converted = Arriba::Text::ASCIIToUnicode(str.c_str());
            _strings.push_back(converted);
            free(converted);
        }
        updateStrings(_strings);
    }

    void InertialList::updateStrings(const std::vector<std::u32string>& strings) {
        root->transform.position.y = 0;
        while (!root->getChildren().empty()) {
            root->getChildren()[0]->destroy();
        }

        itemCount = strings.size();
        for (unsigned int i = 0; i < itemCount; i++) {
            Arriba::Primitives::Text* itemText = new Arriba::Primitives::Text(strings[i].c_str(), 48);
            Arriba::Primitives::Quad* itemContainer = new Arriba::Primitives::Quad(0, i * itemHeight, Quad::width, itemHeight, Arriba::Graphics::Pivot::topLeft);
            itemText->setColour({0.0f, 0.0f, 0.0f, 1.0f});
            itemText->setParent(itemContainer);
            itemText->transform.position.x += itemText->width / 2 + Quad::width * 0.05;
            itemText->transform.position.y += itemHeight / 2;
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
        if (Arriba::highlightedObject == this) {
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
                auto* selectedItemText = static_cast<Arriba::Primitives::Quad*>(root->getChildren()[selectedIndex]->getChildren()[0]);
                float menuX = transform.position.x + (float)selectedItemText->getRight();
                float menuY = root->transform.position.y + (selectedIndex + 1) * itemHeight + itemHeight * 0.5f;
                for (auto& cb : altCallbacks) cb(selectedIndex, Arriba::Maths::vec2<float>{menuX, menuY});
            }

            if (std::abs(Arriba::Input::AnalogStickLeft.yPos + Arriba::Input::AnalogStickRight.yPos) > 0.1f) {
                stickMovementAccumulator += (Arriba::Input::AnalogStickLeft.yPos + Arriba::Input::AnalogStickRight.yPos) * Arriba::deltaTime;
                if (abs(stickMovementAccumulator) > 0.07) {
                    if (stickMovementAccumulator > 0) {
                        selectedIndex -= 1;
                    } else {
                        selectedIndex += 1;
                    }
                    stickMovementAccumulator = 0.0;
                    if (selectedIndex < 0) selectedIndex = 0;
                    if (selectedIndex > itemCount-1) selectedIndex = itemCount-1;
                }
            } else {
                stickMovementAccumulator = 0.0;
            }
        }

        if (Arriba::Input::touchScreenPressed() && Arriba::activeLayer == layer) {
            float touchX = Arriba::Input::touch.pos.x;
            float touchY = Arriba::Input::touch.pos.y;
            if (touchY < getTop() && touchY > getBottom() && touchX < getRight() && touchX > getLeft()) {
                Arriba::highlightedObject = this;
                if (std::abs(Arriba::Input::touch.origin.y - touchY) > itemHeight / 2) {
                    inertia = -Arriba::Input::touch.delta.y;
                    selectedIndex = -1;
                } else {
                    float relativeTouchY = Quad::getBottom() + touchY;
                    for (unsigned int i = 0; i < itemCount+2; i++) {
                        if (relativeTouchY > root->transform.position.y + i * itemHeight && relativeTouchY < root->transform.position.y + (i+1) * itemHeight) {
                            selectedIndex = i - Quad::getBottom() / itemHeight;
                        }
                    }
                }
            } else if (Arriba::highlightedObject == this) {
                highlightedObject = nullptr;
            }
        }

        if (Arriba::highlightedObject == this && Arriba::Input::touch.end && selectedIndex != -1 && activeLayer == layer) {
            if (Arriba::Input::touch.downTime < 0.5f) {
                for (auto& cb : callbacks) cb(selectedIndex);
            } else {
                for (auto& cb : altCallbacks) cb(selectedIndex, Arriba::Input::touch.pos);
            }
        }

        // If selected index is off screen add inertia to bring it on screen
        if (selectedIndex >= 0) {
            // 6.6665 was chosen here because 0.85^x as x goes to infinity converges on 6.6 recurring
            if (selectedIndex * itemHeight + root->transform.position.y < 0) inertia = (selectedIndex * itemHeight + root->transform.position.y) / -6.6665;
            else if ((selectedIndex+1) * itemHeight + root->transform.position.y > Quad::height) inertia = ((selectedIndex+1) * itemHeight + root->transform.position.y - Quad::height) / -6.6665;
        }

        root->transform.position.y += inertia;
        inertia *= 0.85;

        if (root->transform.position.y + itemHeight * itemCount < Quad::height) root->transform.position.y = Quad::height - itemCount * itemHeight;
        if (root->transform.position.y > 0) root->transform.position.y = 0;

        float fadeTime = 3 * Arriba::deltaTime;
        for (unsigned int i = 0; i < itemCount; i++) {
            if (i != selectedIndex) {
                root->getChildren()[i]->setColour(Arriba::Maths::lerp(root->getChildren()[i]->getColour(), Arriba::Colour::neutral, fadeTime));
            } else {
                if (Arriba::highlightedObject != this) {
                    root->getChildren()[i]->setColour(Arriba::Maths::lerp(root->getChildren()[i]->getColour(), Arriba::Colour::highlightB, fadeTime));
                } else {
                    float lerpValue = (sin(Arriba::time*4) + 1) / 2;
                    Arriba::Maths::vec4 targetColour = Arriba::Maths::lerp(Arriba::Colour::highlightA, Arriba::Colour::highlightB, lerpValue);
                    if (selectedIndex != lastSelectedIndex || Arriba::Input::buttonDown(Arriba::Input::controllerButton::AButtonSwitch)) {
                        root->getChildren()[i]->setColour(Arriba::Colour::activatedColour);
                    } else {
                        root->getChildren()[i]->setColour(Arriba::Maths::lerp(root->getChildren()[i]->getColour(), targetColour, fadeTime));
                    }
                }
            }
        }

        // Set bg colour that is seen when there is not enough items
        bg->setColour(Arriba::Maths::lerp(bg->getColour(), Arriba::Colour::neutral, fadeTime));
        lastSelectedIndex = selectedIndex;
        update();
    }

    void InertialList::update() {
        glBindFramebuffer(GL_FRAMEBUFFER, FBO);
        glClearColor(0.0f, 0.3f, 0.6f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);
        drawTextureObject(bg);

        int listItemRenderIndex = -root->transform.position.y / itemHeight;
        int listItemRenderCount = listItemRenderIndex + height / itemHeight + 1;
        if (listItemRenderCount > itemCount) listItemRenderCount = itemCount;
        for (unsigned int i = listItemRenderIndex; i < listItemRenderCount; i++) {
            root->getChildren()[i]->renderer->updateParentTransform(root->renderer->getTransformMatrix());
            drawTextureObject(root->getChildren()[i]);
        }
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
    }

    void InertialList::registerCallback(std::function<void(int)> func) {
        callbacks.push_back(func);
    }

    void InertialList::registerAltCallback(std::function<void(int, Arriba::Maths::vec2<float>)> func) {
        altCallbacks.push_back(func);
    }
} // namespace Arriba::Elements
