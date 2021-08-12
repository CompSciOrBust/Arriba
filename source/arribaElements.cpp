#include <arribaElements.h>

Arriba::Elements::Button::Button() : Arriba::Primitives::Quad(0,0,0,0, Arriba::Graphics::Pivot::centre)
{
    text = new Arriba::Primitives::Text("Default", 48);
    text->setParent(this);
    setDimensions(200, 200, Arriba::Graphics::Pivot::centre);
    text->transform.position = {(getRight() + getLeft())/2, (getTop() + getBottom())/2, 0};
}

void Arriba::Elements::Button::onFrame()
{
    //When A pressed call all callbacks
    if(Arriba::Input::buttonDown(HidNpadButton_A) && Arriba::highlightedObject == this)
    {
        for (unsigned int i = 0; i < callbacks.size(); i++)
        {
            callbacks[i]();
        }
    }
    //When tapped call all callbacks and highlight self
    bool isTouched = false;
    if(Arriba::Input::touchScreenPressed() && Arriba::Input::touch.start)
    {
        glm::vec3 pos = glm::vec3(getGlobalPos()[3]);
        float touchX = Arriba::Input::touch.pos.x;
        float touchY = Arriba::Input::touch.pos.y;
        isTouched = touchY < getTop() && touchY > getBottom() && touchX < getRight() && touchX > getLeft();
        if(isTouched)
        {
            Arriba::highlightedObject = this;
            for (unsigned int i = 0; i < callbacks.size(); i++)
            {
                callbacks[i]();
            }
        }
        else if(Arriba::highlightedObject == this) Arriba::highlightedObject = 0;
    }
    //When highlighted cycle between neutral and highlighted colour
    glm::vec4 targetColour = neutral;
    float lerpValue = (sin(time*4) + 1) / 2;
    if(Arriba::highlightedObject == this)
    {
        time += Arriba::deltaTime;
        targetColour = glm::mix(highlightA, highlightB, lerpValue);
        if(Arriba::Input::buttonDown(HidNpadButton_A) || isTouched) setColour(activatedColour);
    }
    //Slowly transition to the target colour
    setColour(glm::mix(getColour(), targetColour, 0.375)); //This should account for delta time but doesn't because it causes bugs
}

void Arriba::Elements::Button::setText(const char* _text)
{
    text->setText(_text);
}

void Arriba::Elements::Button::registerCallback(void (*func)())
{
    callbacks.push_back(func);
}