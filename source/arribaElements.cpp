#include <arribaElements.h>

Arriba::Elements::Button::Button() : Arriba::Primitives::Quad(0,0,0,0, Arriba::Graphics::Pivot::centre)
{
    text = new Arriba::Primitives::Text("Default", 48);
    text->setParent(this);
    setDimensions(200, 200, Arriba::Graphics::Pivot::centre);
    text->transform.position = {(right + left)/2, (top + bottom)/2, 0};
    setColour(Arriba::Colour::neutral);
}

void Arriba::Elements::Button::onFrame()
{
    //"Temporary" hack to avoid rewriting buttons to use framebuffers
    text->transform.position = {(right + left)/2, (top + bottom)/2, 0};
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
    glm::vec4 targetColour = Arriba::Colour::neutral;
    float lerpValue = (sin(Arriba::time*4) + 1) / 2;
    if(Arriba::highlightedObject == this)
    {
        targetColour = glm::mix(Arriba::Colour::highlightA, Arriba::Colour::highlightB, lerpValue);
        if(Arriba::Input::buttonDown(HidNpadButton_A) || isTouched) setColour(Arriba::Colour::activatedColour);
    }
    //Slowly transition to the target colour
    float fadeTime = 3 * Arriba::deltaTime;
    setColour(glm::mix(getColour(), targetColour, fadeTime)); //This should account for delta time but doesn't because it causes bugs
}

void Arriba::Elements::Button::setText(const char* _text)
{
    text->setText(_text);
}

void Arriba::Elements::Button::registerCallback(void (*func)())
{
    callbacks.push_back(func);
}

Arriba::Elements::InertialList::InertialList(int _x, int _y, int _width, int _height, std::vector<std::string> strings) : Arriba::Graphics::AdvancedTexture(_width, _height), Arriba::Primitives::Quad(_x, _y, _width, _height, Arriba::Graphics::Pivot::topLeft)
{
    renderer->setTexture(texID);
    root = new Arriba::Primitives::Quad(0,0,0,0,Arriba::Graphics::Pivot::centre);
    root->setColour({0.0f,0.0f,0.0f,0.0f});
    updateStrings(strings);
    root->setFBOwner(this);
}

Arriba::Elements::InertialList::~InertialList()
{
    root->destroy();
}

void Arriba::Elements::InertialList::updateStrings(std::vector<std::string> strings)
{
    for (unsigned int i = 0; i < root->getChildren().size(); i++)
    {
        root->getChildren()[0]->destroy();
    }
    itemCount = strings.size();
    for (unsigned int i = 0; i < itemCount; i++)
    {
        Arriba::Primitives::Text* itemText = new Arriba::Primitives::Text(strings[i].c_str(), 48);
        Arriba::Primitives::Quad* itemContainer = new Arriba::Primitives::Quad(0, i * itemHeight, Quad::width, itemHeight, Arriba::Graphics::Pivot::topLeft);
        itemText->setColour({0.0f, 0.0f, 0.0f, 1.0f});
        itemText->setParent(itemContainer);
        itemText->transform.position.x += itemText->width / 2 + Quad::width * 0.05;
        itemText->transform.position.y += itemHeight / 2;
        itemContainer->setParent(root);
        itemContainer->setColour(Arriba::Colour::neutral);
    }
}

void Arriba::Elements::InertialList::onFrame()
{
    //Handle button input
    if(Arriba::highlightedObject == this)
    {
        //Up pressed
        if(Arriba::Input::buttonDown(HidNpadButton_Up))
        {
            selectedIndex -= 1;
            if(selectedIndex < 0) selectedIndex = 0;
        }
        //Down pressed
        if(Arriba::Input::buttonDown(HidNpadButton_Down))
        {
            selectedIndex += 1;
            if(selectedIndex > root->getChildren().size()-1) selectedIndex = root->getChildren().size()-1;
        }
        //A pressed
        if(Arriba::Input::buttonDown(HidNpadButton_A) && selectedIndex != -1)
        {
            for (unsigned int i = 0; i < callbacks.size(); i++)
            {
                callbacks[i](selectedIndex);
            }
        }
    }
    //Handle touch input
    if(Arriba::Input::touchScreenPressed())
    {
        //Check if list is touched
        glm::vec3 pos = glm::vec3(getGlobalPos()[3]);
        float touchX = Arriba::Input::touch.pos.x;
        float touchY = Arriba::Input::touch.pos.y;
        //Yes the list is touched
        if(touchY < getTop() && touchY > getBottom() && touchX < getRight() && touchX > getLeft())
        {
            Arriba::highlightedObject = this;
            if(abs(Arriba::Input::touch.origin.y - touchY) > itemHeight / 2)
            {
                inertia = -Arriba::Input::touch.delta.y;
                selectedIndex = -1;
            }
            else
            {
                //Highlight correct item when tapped
                float relativeTouchY = Quad::getBottom() + touchY;
                for (unsigned int i = 0; i < itemCount+1; i++) if(relativeTouchY > root->transform.position.y + i * itemHeight) selectedIndex = i-1;
                //This is a lot nicer than the hack above but it has an off by one error when there is half an item on screen.
                //selectedIndex = int(round(Quad::height / itemHeight)) - int((Quad::getTop() - touchY + root->transform.position.y) / itemHeight);
            }
        }
        else if(Arriba::highlightedObject == this) highlightedObject = 0;
    }
    //Callback for touch finished
    if(Arriba::highlightedObject == this && Arriba::Input::touch.end && selectedIndex != -1)
    {
        for (unsigned int i = 0; i < callbacks.size(); i++)
        {
            callbacks[i](selectedIndex);
        }
    }
    //If selected index is off screen add inertia to bring it on screen
    if(selectedIndex >= 0)
    {
        if(selectedIndex * itemHeight + root->transform.position.y < 0) inertia += 2;
        else if((selectedIndex+1) * itemHeight + root->transform.position.y > Quad::height) inertia -= 2;
    }
    //Deal with inertia
    root->transform.position.y += inertia;
    inertia *= 0.8;
    //Make sure that the root stays within bounds
    //Make sure root doesn't go too high
    if(root->transform.position.y > 0) root->transform.position.y = 0;
    //Make sure root doesn't go too low
    if(root->transform.position.y + itemHeight * itemCount < Quad::height) root->transform.position.y = Quad::height - itemCount * itemHeight;
    //Highlight selected index
    float fadeTime = 3 * Arriba::deltaTime;
    //Loop through each item
    for (unsigned int i = 0; i < itemCount; i++)
    {
        //If this is not the selected item fade to neutral
        if(i != selectedIndex) root->getChildren()[i]->setColour(glm::mix(root->getChildren()[i]->getColour(),Arriba::Colour::neutral,fadeTime));
        else
        {
            //If this is the selected item but the list is not highlighted fade to highlight b
            if(Arriba::highlightedObject != this) root->getChildren()[i]->setColour(glm::mix(root->getChildren()[i]->getColour(),Arriba::Colour::highlightB,fadeTime));
            else //If this is the selected item pulse between highlight a and highlight b
            {
                float lerpValue = (sin(Arriba::time*4) + 1) / 2;
                glm::vec4 targetColour = glm::mix(Arriba::Colour::highlightA, Arriba::Colour::highlightB, lerpValue);
                //If A was pressed or item is newly selected pulse the activated colour
                if(selectedIndex != lastSelectedIndex || Arriba::Input::buttonDown(HidNpadButton_A)) root->getChildren()[i]->setColour(Arriba::Colour::activatedColour);
                else root->getChildren()[i]->setColour(glm::mix(root->getChildren()[i]->getColour(),targetColour,fadeTime));
            }
        }
    }
    lastSelectedIndex = selectedIndex;
    update();
}

void Arriba::Elements::InertialList::update()
{
    //Render framebuffer
    glBindFramebuffer(GL_FRAMEBUFFER, FBO);
    glClearColor(0.0f, 0.3f, 0.6f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);
    drawTextureObject(root);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void Arriba::Elements::InertialList::registerCallback(void (*func)(int))
{
    callbacks.push_back(func);
}