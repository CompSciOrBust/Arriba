#include <arribaElements.h>

namespace Arriba::Elements {
    Button::Button() : Arriba::Primitives::Quad(0, 0, 0, 0, Arriba::Graphics::Pivot::centre) {
        text = new Arriba::Primitives::Text("Default", 48);
        text->setParent(this);
        setDimensions(200, 200, Arriba::Graphics::Pivot::centre);
        text->transform.position = {(right + left)/2, (top + bottom)/2, 0};
        setColour(Arriba::Colour::neutral);
    }

    void Button::onFrame() {
        // "Temporary" hack to avoid rewriting buttons to use framebuffers
        text->transform.position = {(right + left)/2, (top + bottom)/2, 0};
        // When A pressed call all callbacks
        if (Arriba::Input::buttonDown(Arriba::Input::controllerButton::AButtonSwitch) && Arriba::highlightedObject == this) {
            for (unsigned int i = 0; i < callbacks.size(); i++) {
                callbacks[i]();
            }
        }
        // When tapped call all callbacks and highlight self
        bool isTouched = false;
        if (Arriba::Input::touchScreenPressed() && Arriba::Input::touch.start && activeLayer == layer) {
            Arriba::Maths::vec3<float> pos = getGlobalPos().col4;
            float touchX = Arriba::Input::touch.pos.x;
            float touchY = Arriba::Input::touch.pos.y;
            isTouched = touchY < getTop() && touchY > getBottom() && touchX < getRight() && touchX > getLeft();
            if (isTouched) {
                Arriba::highlightedObject = this;
                for (unsigned int i = 0; i < callbacks.size(); i++) {
                    callbacks[i]();
                }
            } else if (Arriba::highlightedObject == this) {
                Arriba::highlightedObject = 0;
            }
        }
        // When highlighted cycle between neutral and highlighted colour
        Arriba::Maths::vec4 targetColour = Arriba::Colour::neutral;
        float lerpValue = (sin(Arriba::time*4) + 1) / 2;
        if (Arriba::highlightedObject == this) {
            targetColour = Arriba::Maths::lerp(Arriba::Colour::highlightA, Arriba::Colour::highlightB, lerpValue);
            if (Arriba::Input::buttonDown(Arriba::Input::controllerButton::AButtonSwitch) || isTouched) setColour(Arriba::Colour::activatedColour);
        }
        // Slowly transition to the target colour
        float fadeTime = 3 * Arriba::deltaTime;
        setColour(Arriba::Maths::lerp(getColour(), targetColour, fadeTime));
    }

    void Button::setText(const char* _text) {
        text->setText(_text);
    }

    void Button::registerCallback(void (*func)()) {
        callbacks.push_back(func);
    }

    InertialList::InertialList(int _x, int _y, int _width, int _height, std::vector<std::string> strings) : Arriba::Graphics::AdvancedTexture(_width, _height), Arriba::Primitives::Quad(_x, _y, _width, _height, Arriba::Graphics::Pivot::topLeft) {
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

    void InertialList::updateStrings(std::vector<std::string> strings) {
        root->transform.position.y = 0;
        for (unsigned int i = 0; i < itemCount; i++) {
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
        // Handle button / stick input
        if (Arriba::highlightedObject == this) {
            // Up pressed
            if (Arriba::Input::buttonDown(Arriba::Input::controllerButton::DPadUp)) {
                if (selectedIndex >= 0 || itemCount * itemHeight < Quad::height) {
                    selectedIndex -= 1;
                    if (selectedIndex < 0) selectedIndex = itemCount - 1;
                } else {
                    // No item is currently selected but don't just jump to index 0
                    selectedIndex = static_cast<int>(-root->transform.position.y / itemHeight);
                }
            }
            // Down pressed
            if (Arriba::Input::buttonDown(Arriba::Input::controllerButton::DPadDown)) {
                if (selectedIndex >= 0 || itemCount * itemHeight < Quad::height) {
                    selectedIndex += 1;
                    if (selectedIndex > itemCount-1) selectedIndex = 0;
                } else {
                    // No item is currently selected so select item at bottom of list
                    selectedIndex = static_cast<int>(-root->transform.position.y / itemHeight);
                }
            }

            // A pressed
            if (Arriba::Input::buttonDown(Arriba::Input::controllerButton::AButtonSwitch) && selectedIndex != -1) {
                for (unsigned int i = 0; i < callbacks.size(); i++) {
                    callbacks[i](selectedIndex);
                }
            }
            // Y pressed
            if (Arriba::Input::buttonDown(Arriba::Input::controllerButton::YButtonSwitch) && selectedIndex != -1) {
                for (unsigned int i = 0; i < altCallbacks.size(); i++) {
                    // The line below is gross but it works
                    // To get the vec2 we take the x pos of the list, find the text child of the indexed list item, then take its right x value and add it to what we have
                    // To get the Y value we take the y pos of the list and add the y pos of the currently selected list item and add have of the item height to get the center of the quad
                    altCallbacks[i](selectedIndex, Arriba::Maths::vec2<float>{transform.position.x + (float)static_cast<Arriba::Primitives::Quad*>(root->getChildren()[selectedIndex]->getChildren()[0])->getRight(), root->transform.position.y + (selectedIndex + 1) * itemHeight + itemHeight * 0.5});
                }
            }

            // Stick movement
            if (abs(Arriba::Input::AnalogStickLeft.yPos + Arriba::Input::AnalogStickRight.yPos) > 0.1) {
                // Build up the movement acumulator
                stickMovementAcumulator += (Arriba::Input::AnalogStickLeft.yPos + Arriba::Input::AnalogStickRight.yPos) * Arriba::deltaTime;
                if (abs(stickMovementAcumulator) > 0.07) {
                    if (stickMovementAcumulator > 0) {
                        selectedIndex -= 1;
                    } else {
                        selectedIndex += 1;
                    }
                    stickMovementAcumulator = 0.0;
                    if (selectedIndex < 0) selectedIndex = 0;
                    if (selectedIndex > itemCount-1) selectedIndex = itemCount-1;
                }
            } else {
                // Remove any accumulation
                stickMovementAcumulator = 0.0;
            }
        }

        // Handle touch input
        if (Arriba::Input::touchScreenPressed() && Arriba::activeLayer == layer) {
            // Check if list is touched
            Arriba::Maths::vec3<float> pos = getGlobalPos().col4;
            float touchX = Arriba::Input::touch.pos.x;
            float touchY = Arriba::Input::touch.pos.y;
            // Yes the list is touched
            if (touchY < getTop() && touchY > getBottom() && touchX < getRight() && touchX > getLeft()) {
                Arriba::highlightedObject = this;
                if (abs(Arriba::Input::touch.origin.y - touchY) > itemHeight / 2) {
                    inertia = -Arriba::Input::touch.delta.y;
                    selectedIndex = -1;
                } else {
                    // Highlight correct item when tapped
                    float relativeTouchY = Quad::getBottom() + touchY;
                    for (unsigned int i = 0; i < itemCount+2; i++) {
                        if (relativeTouchY > root->transform.position.y + i * itemHeight && relativeTouchY < root->transform.position.y + (i+1) * itemHeight) {
                            selectedIndex = i - Quad::getBottom() / itemHeight;
                        }
                    }
                    // This is a lot nicer than the hack above but it has an off by one error when there is half an item on screen.
                    // selectedIndex = int(round(Quad::height / itemHeight)) - int((Quad::getTop() - touchY + root->transform.position.y) / itemHeight);
                }
            } else if (Arriba::highlightedObject == this) {
                highlightedObject = 0;
            }
        }

        // Callback for touch finished
        if (Arriba::highlightedObject == this && Arriba::Input::touch.end && selectedIndex != -1 && activeLayer == layer) {
            // Short tap callbacks
            if (Arriba::Input::touch.downTime < 0.5) {
                for (unsigned int i = 0; i < callbacks.size(); i++) {
                    callbacks[i](selectedIndex);
                }
            } else {
                // Long press callbacks
                for (unsigned int i = 0; i < altCallbacks.size(); i++) {
                    altCallbacks[i](selectedIndex, Arriba::Input::touch.pos);
                }
            }
        }

        // If selected index is off screen add inertia to bring it on screen
        if (selectedIndex >= 0) {
            // 6.6665 was chosen here because 0.85^x as x goes to infinity converges on 6.6 recurring
            if (selectedIndex * itemHeight + root->transform.position.y < 0) inertia = (selectedIndex * itemHeight + root->transform.position.y) / -6.6665;
            else if ((selectedIndex+1) * itemHeight + root->transform.position.y > Quad::height) inertia = ((selectedIndex+1) * itemHeight + root->transform.position.y - Quad::height) / -6.6665;
        }

        // Deal with inertia
        root->transform.position.y += inertia;
        inertia *= 0.85;
        // Make sure that the root stays within bounds
        // Make sure root doesn't go too low
        if (root->transform.position.y + itemHeight * itemCount < Quad::height) root->transform.position.y = Quad::height - itemCount * itemHeight;
        // Make sure root doesn't go too high
        if (root->transform.position.y > 0) root->transform.position.y = 0;
        // Highlight selected index
        float fadeTime = 3 * Arriba::deltaTime;
        // Loop through each item
        for (unsigned int i = 0; i < itemCount; i++) {
            // If this is not the selected item fade to neutral
            if (i != selectedIndex) {
                root->getChildren()[i]->setColour(Arriba::Maths::lerp(root->getChildren()[i]->getColour(), Arriba::Colour::neutral, fadeTime));
            } else {
                // If this is the selected item but the list is not highlighted fade to highlight b
                if (Arriba::highlightedObject != this) {
                    root->getChildren()[i]->setColour(Arriba::Maths::lerp(root->getChildren()[i]->getColour(), Arriba::Colour::highlightB, fadeTime));
                } else {
                    // If this is the selected item pulse between highlight a and highlight b
                    float lerpValue = (sin(Arriba::time*4) + 1) / 2;
                    Arriba::Maths::vec4 targetColour = Arriba::Maths::lerp(Arriba::Colour::highlightA, Arriba::Colour::highlightB, lerpValue);
                    // If A was pressed or item is newly selected pulse the activated colour
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
        // Render framebuffer
        glBindFramebuffer(GL_FRAMEBUFFER, FBO);
        glClearColor(0.0f, 0.3f, 0.6f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);
        drawTextureObject(bg);
        // Find indexes of list items that are on screen
        int listItemRenderIndex = -root->transform.position.y / itemHeight;
        int listItemRenderCount = listItemRenderIndex + height / itemHeight + 1;
        if (listItemRenderCount > itemCount) listItemRenderCount = itemCount;
        // Draw on screen list items
        for (unsigned int i = listItemRenderIndex; i < listItemRenderCount; i++) {
            root->getChildren()[i]->renderer->updateParentTransform(root->renderer->getTransformMatrix());
            drawTextureObject(root->getChildren()[i]);
        }
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
    }

    void InertialList::registerCallback(void (*func)(int)) {
        callbacks.push_back(func);
    }

    void InertialList::registerAltCallback(void (*func)(int, Arriba::Maths::vec2<float>)) {
        altCallbacks.push_back(func);
    }
}  // namespace Arriba::Elements
