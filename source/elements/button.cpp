#include <elements/button.h>
#include <arribaText.h>

namespace Arriba::Elements {
    Button::Button() : Arriba::Primitives::Quad(0, 0, 0, 0, Arriba::Graphics::Pivot::centre) {
        text = new Arriba::Primitives::Text(U"Default", 48);
        text->setParent(this);
        setDimensions(200, 200, Arriba::Graphics::Pivot::centre);
        text->transform.position = {(right + left)/2, (top + bottom)/2, 0};
        setColour(Arriba::Colour::neutral);
    }

    void Button::onFrame() {
        // "Temporary" hack to avoid rewriting buttons to use framebuffers
        text->transform.position = {(right + left)/2, (top + bottom)/2, 0};

        if (Arriba::Input::buttonDown(Arriba::Input::controllerButton::AButtonSwitch) && Arriba::highlightedObject == this) {
            for (auto& cb : callbacks) cb();
        }

        float touchX = Arriba::Input::touch.pos.x;
        float touchY = Arriba::Input::touch.pos.y;
        bool isTouched = false;
        if (Arriba::Input::touchScreenPressed() && activeLayer == layer) {
            bool withinBounds = touchY < getTop() && touchY > getBottom() && touchX < getRight() && touchX > getLeft();
            if (Arriba::Input::touch.start) {
                if (withinBounds) Arriba::highlightedObject = this;
                else if (Arriba::highlightedObject == this) Arriba::highlightedObject = nullptr;
            }
            if (Arriba::highlightedObject == this) isTouched = withinBounds;
        }

        if (Arriba::highlightedObject == this && Arriba::Input::touch.end && activeLayer == layer) {
            if (touchY < getTop() && touchY > getBottom() && touchX < getRight() && touchX > getLeft()) {
                for (auto& cb : callbacks) cb();
            }
        }

        Arriba::Maths::vec4 targetColour = Arriba::Colour::neutral;
        float lerpValue = (sin(Arriba::time*4) + 1) / 2;
        if (Arriba::highlightedObject == this) {
            targetColour = Arriba::Maths::lerp(Arriba::Colour::highlightA, Arriba::Colour::highlightB, lerpValue);
            if (Arriba::Input::buttonDown(Arriba::Input::controllerButton::AButtonSwitch) || isTouched) setColour(Arriba::Colour::activatedColour);
        }
        float fadeTime = 3 * Arriba::deltaTime;
        setColour(Arriba::Maths::lerp(getColour(), targetColour, fadeTime));
    }

    void Button::setText(const char* text) {
        setText(Arriba::Text::ASCIIToUnicode(text).c_str());
    }

    void Button::setText(const char32_t* text) {
        this->text->setText(text);
    }

    void Button::registerCallback(std::function<void()> func) {
        callbacks.push_back(func);
    }
}  // namespace Arriba::Elements
