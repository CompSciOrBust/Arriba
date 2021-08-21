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

Arriba::Elements::InertialListTexture::InertialListTexture(int _x, int _y, int _width, int _height, std::vector<std::string> strings) : Arriba::Graphics::AdvancedTexture(_width, _height), Arriba::Primitives::Quad(_x, _y, _width, _height, Arriba::Graphics::Pivot::topLeft)
{
    renderer->setTexture(texID);
    root = new Arriba::Primitives::Quad(0,0,0,0,Arriba::Graphics::Pivot::centre);
    root->setColour({0.0f,0.0f,0.0f,0.0f});
    itemCount = strings.size();
    for (unsigned int i = 0; i < strings.size(); i++)
    {
        Arriba::Primitives::Text* itemText = new Arriba::Primitives::Text(strings[i].c_str(), 48);
        Arriba::Primitives::Quad* itemContainer = new Arriba::Primitives::Quad(0, i * itemHeight, _width, itemHeight, Arriba::Graphics::Pivot::topLeft);
        //itemText->setDimensions(itemText->width, itemText->height, Arriba::Graphics::Pivot::centre);
        //itemText->setFBOwner(this);
        //itemText->transform.position.y = itemText.h
        itemText->setColour({0.0f, 0.0f, 0.0f, 1.0f});
        itemText->setParent(itemContainer);
        itemText->transform.position.x += itemText->width / 2 + _width * 0.05;
        itemText->transform.position.y += itemHeight / 2;
        itemContainer->setParent(root);
    }
    root->setFBOwner(this);
}

Arriba::Elements::InertialListTexture::~InertialListTexture()
{
    root->destroy();
}

void Arriba::Elements::InertialListTexture::onFrame()
{
    Arriba::highlightedObject = this; //Debug
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
                selectedIndex = int(round(Quad::height / itemHeight)) - int((Quad::getTop() - touchY + root->transform.position.y) / itemHeight);
            }
        }
        else if(Arriba::highlightedObject == this) highlightedObject = 0;
    }
    //Deal with inertia
    root->transform.position.y += inertia;
    inertia *= 0.8;
    //Make sure that the root stays within bounds
    //Make sure list doesn't go too high
    if(root->transform.position.y > 0) root->transform.position.y = 0;
    //Make sure list doesn't go too low
    if(root->transform.position.y + itemHeight * itemCount < Quad::height) root->transform.position.y = Quad::height - itemCount * itemHeight;
    //Highlight selected index
    if(lastSelectedIndex >= 0) root->getChildren()[lastSelectedIndex]->setColour(neutral);
    if(selectedIndex >= 0) root->getChildren()[selectedIndex]->setColour(highlightA);
    lastSelectedIndex = selectedIndex;
    update();
}

void Arriba::Elements::InertialListTexture::update()
{
    //Render framebuffer
    glBindFramebuffer(GL_FRAMEBUFFER, FBO);
    glClearColor(0.0f, 0.3f, 0.6f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);
    drawTextureObject(root);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}