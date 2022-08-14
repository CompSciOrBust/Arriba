#include <arriba.h>
#include <stdlib.h>
#include <time.h>
#include <vector>
#include <chrono>

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#ifdef __SWITCH__
#include <switch.h>
#endif

namespace Arriba
{
    void init()
    {
        printf("Initializing\n");
        if(Arriba::Graphics::graphicsAreInitialised != true)
        {
            Arriba::Graphics::initGraphics();
            //On Switch set up hook for docking / undocking
            #ifdef __SWITCH__
            Arriba::Graphics::dockStatusCallback(AppletHookType_OnOperationMode, nullptr);
            appletHook(&switchDockCookie, Arriba::Graphics::dockStatusCallback, nullptr);
            #endif
            Arriba::Input::initInput();
            lastFrameTime = armTicksToNs(armGetSystemTick());
        }
        printf("Init done!\n");
    }

    //This only exists because AtlasNX people bullied me
    void exit()
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
        //On switch unhook dock event
        #ifdef __SWITCH__
            appletUnhook(&switchDockCookie);
        #endif
    }

    void drawFrame()
    {
        //Prevent force closing the app
        appletLockExit();
        //Calculate delta time
        double msSinceLastFrame = (armTicksToNs(armGetSystemTick()) - lastFrameTime) / 1000000;
        deltaTime = msSinceLastFrame / 1000;
        time += deltaTime;
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

    void drawFrameActions(UIObject* object)
    {
        //Skip if disabled or belongs to framebuffer
        if(object->enabled == false || object->renderer->FBOwner != nullptr) return;
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

    void drawTextureObject(UIObject* object)
    {
        //Skip if disabled
        if(object->enabled == false) return;
        //Render object
        object->renderer->renderObject();
        //Do same for all children
        for (unsigned int i = 0; i < object->getChildren().size(); i++)
        {
            object->getChildren()[i]->renderer->updateParentTransform(object->renderer->getTransformMatrix());
            drawTextureObject(object->getChildren()[i]);
        }
    }

    void Behaviour::attachToObject(UIObject* obj)
    {
        object = obj;
    }

    void UIObject::removeChild(int ID)
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

    void UIObject::addBehaviour(Behaviour* behaviour)
    {
        behaviours.push_back(behaviour);
        behaviour->attachToObject(this);
    }

    UIObject* Arriba::UIObject::getParent()
    {
        return parent;
    }

    std::vector<Arriba::UIObject*> UIObject::getChildren()
    {
        return children;
    }

    std::vector<Arriba::Behaviour*> UIObject::getBehaviours()
    {
        return behaviours;
    }

    void UIObject::setColour(Arriba::Maths::vec4<float> _colour)
    {
        renderer->setColour(_colour);
    }

    Arriba::Maths::vec4<float> UIObject::getColour()
    {
        return renderer->getColour();
    }

    Arriba::Maths::mat4<float> UIObject::getGlobalPos()
    {
        return renderer->getTransformMatrix();
    }

    void UIObject::setFBOwner(Arriba::Graphics::AdvancedTexture* fb)
    {
        this->renderer->FBOwner = fb;
        for (unsigned int i = 0; i < children.size(); i++)
        {
            children[i]->setFBOwner(fb);
        }
    }

    void UIObject::destroy()
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

    void UIObject::setParent(Arriba::UIObject* newParent)
    {
        //Remove self from parent's children
        if(parent) parent->removeChild(objectID);
        //Add self to new parent's children and adopt their framebuffer owner
        if(newParent)
        {
            newParent->children.push_back(this);
            setFBOwner(newParent->renderer->FBOwner);
        }
        else setFBOwner(nullptr); //Set framebuffer owner to nullptr if no parent
        //Set parent to new parent
        parent = newParent;
    }

    UIObject::UIObject()
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

    UIObject::~UIObject()
    {
        //renderer.release();
    }

    UIObject* findObjectByName(std::string name)
    {
        for (size_t i = 0; i < objectList.size(); i++)
        {
            if(objectList[i]->name == name) return objectList[i].get();
        }
    }

    std::vector<UIObject*> findObjectsByTag(std::string tag)
    {
        std::vector<UIObject*> list = {};
        for (size_t i = 0; i < objectList.size(); i++)
        {
            if(objectList[i]->tag == tag) list.push_back(objectList[i].get());
        }
        return list;
    }
}