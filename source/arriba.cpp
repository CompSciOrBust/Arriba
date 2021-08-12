#include <arriba.h>
#include <stdlib.h>
#include <time.h>
#include <vector>
#include <chrono>

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <switch.h>

void Arriba::init()
{
    printf("Initializing\n");
    if(Arriba::Graphics::graphicsAreInitialised != true)
    {
        Arriba::Graphics::initGraphics();
        Arriba::Input::initInput();
    }
    printf("Init done!\n");
}

//This only exists because AtlasNX people bullied me
void Arriba::exit()
{
    //Delete all loaded objects
    while(objectList.size() != 0)
    {
        objectList[0]->destroy();
    }
    //Destroy default shaders
    glDeleteProgram(Arriba::Graphics::defaultShaderID);
    glDeleteProgram(Arriba::Graphics::textShaderID);
    //Kill GLFW
    glfwTerminate();
}

void Arriba::drawFrame()
{
    //Prevent force closing the app
    appletLockExit();
    //Calculate delta time
    double msSinceLastFrame = (armTicksToNs(armGetSystemTick()) - lastFrameTime) / 1000000;
    deltaTime = msSinceLastFrame / 1000;
    lastFrameTime = armTicksToNs(armGetSystemTick());
    //Update input
    Arriba::Input::updateHID();
    //Clear the screen
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);
    //Render each game object
    for (size_t i = 0; i < objectList.size(); i++)
    {
        //Skip if has parent
        if(objectList[i]->getParent() != nullptr) continue;
        drawFrameActions(objectList[i].get());
    }
    //Update screen
    glfwSwapBuffers(Arriba::Graphics::window);
    glfwPollEvents();
    //Allow force closing the app
    appletUnlockExit();
}

void Arriba::drawFrameActions(UIObject* object)
{
    //Skip if disabled
    if(object->enabled == false) return;
    object->onFrame(); //Perform actions
    //Call update on each behaviour
    for (unsigned int i = 0; i < object->getBehaviours().size(); i++)
    {
        object->getBehaviours()[i]->update();
    }
    object->renderer->renderObject(); //Render
    //Do the same for each child
    for (unsigned int i = 0; i < object->getChildren().size(); i++)
    {
        object->getChildren()[i]->renderer->updateParentTransform(object->renderer->getTransformMatrix());
        drawFrameActions(object->getChildren()[i]);
    }
}

void Arriba::Behaviour::attachToObject(UIObject* obj)
{
    object = obj;
}

void Arriba::UIObject::removeChild(int ID)
{
    for (size_t i = 0; i < children.size(); i++)
    {
        if(children[i]->objectID == ID)
        {
            children.erase(children.begin() + i);
            return;
        }
    }
}

void Arriba::UIObject::addBehaviour(Behaviour* behaviour)
{
    behaviours.push_back(behaviour);
    behaviour->attachToObject(this);
}

Arriba::UIObject* Arriba::UIObject::getParent()
{
    return parent;
}

std::vector<Arriba::UIObject*> Arriba::UIObject::getChildren()
{
    return children;
}

std::vector<Arriba::Behaviour*> Arriba::UIObject::getBehaviours()
{
    return behaviours;
}

void Arriba::UIObject::setColour(glm::vec4 _colour)
{
    renderer->setColour(_colour);
}

glm::vec4 Arriba::UIObject::getColour()
{
    return renderer->getColour();
}

glm::mat4 Arriba::UIObject::getGlobalPos()
{
    return renderer->getTransformMatrix();
}

void Arriba::UIObject::destroy()
{
    setParent(nullptr);
    while (getChildren().size() != 0)
    {
        getChildren()[0]->destroy();
    }
    for (unsigned int i = 0; i < Arriba::objectList.size(); i++)
    {
        if(Arriba::objectList[i].get() == this)
        {
            Arriba::objectList[i].reset();
            Arriba::objectList.erase(Arriba::objectList.begin() + i);
            break;
        }
    }
}

void Arriba::UIObject::setParent(Arriba::UIObject* newParent)
{
    //Remove self from parent's children
    if(parent) parent->removeChild(objectID);
    //Add self to new parent's children
    if(newParent) newParent->children.push_back(this);
    //Set parent to new parent
    parent = newParent;
}

Arriba::UIObject::UIObject()
{
    //Add object to the object list
    objectList.push_back(std::unique_ptr<UIObject>(this));
    //Generate a unique ID and seed random
    objectID = rand();
    srand(rand());
    //Create a renderer
    renderer = std::make_unique<Arriba::Graphics::Renderer>();
    renderer->transform = &transform;
}

Arriba::UIObject::~UIObject()
{
    //renderer.release();
}

Arriba::UIObject* Arriba::findObjectByName(std::string name)
{
    for (size_t i = 0; i < Arriba::objectList.size(); i++)
    {
        if(Arriba::objectList[i]->name == name) return Arriba::objectList[i].get();
    }
}

std::vector<Arriba::UIObject*> Arriba::findObjectsByTag(std::string tag)
{
    std::vector<Arriba::UIObject*> list = {};
    for (size_t i = 0; i < Arriba::objectList.size(); i++)
    {
        if(Arriba::objectList[i]->tag == tag) list.push_back(Arriba::objectList[i].get());
    }
    return list;
}