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
        char32_t* converted = Arriba::Text::ASCIIToUnicode(_text);
        setText(converted);
        free(converted);
    }
    
    void Button::setText(const char32_t* _text) {
        text->setText(_text);
    }

    void Button::registerCallback(void (*func)()) {
        callbacks.push_back(func);
    }
}  // namespace Arriba::Elements
