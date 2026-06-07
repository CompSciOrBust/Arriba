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

namespace Arriba {
    void init() {
        printf("Initializing\n");
        if (Arriba::Graphics::graphicsAreInitialised != true) {
            srand(::time(NULL));
            Arriba::Graphics::initGraphics();
            #ifdef __SWITCH__
            Arriba::Graphics::dockStatusCallback(AppletHookType_OnOperationMode, nullptr);
            appletHook(&switchDockCookie, Arriba::Graphics::dockStatusCallback, nullptr);
            #endif
            Arriba::Input::initInput();
            lastFrameTime = armTicksToNs(armGetSystemTick());
            appletInitializeGamePlayRecording();
        }
        printf("Init done!\n");
    }

    void exit() {
        objectList.clear();
        
        glDeleteProgram(Arriba::Graphics::defaultShaderID);
        glDeleteProgram(Arriba::Graphics::textShaderID);
        
        glfwTerminate();
        
        #ifdef __SWITCH__
            appletUnhook(&switchDockCookie);
        #endif
    }

    void drawFrame() {
        #ifdef __SWITCH__
        appletLockExit();
        #endif

        double msSinceLastFrame = (armTicksToNs(armGetSystemTick()) - lastFrameTime) / 1000000;
        deltaTime = msSinceLastFrame / 1000;
        time += deltaTime;
        lastFrameTime = armTicksToNs(armGetSystemTick());

        Arriba::Input::updateHID();

        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        for (size_t i = 0; i < objectList.size(); i++) {
            if (objectList[i]->getParent() != nullptr) continue;
            if (pendingDestroySet.count(objectList[i].get())) continue;
            drawFrameActions(objectList[i].get());
        }

        glfwSwapBuffers(Arriba::Graphics::window);
        glfwPollEvents();

        for (int i = objectList.size() - 1; i >= 0; i--) {
            if (pendingDestroySet.count(objectList[i].get())) objectList.erase(objectList.begin() + i);
        }
        pendingDestroySet.clear();

        #ifdef __SWITCH__
        appletUnlockExit();
        #endif
    }

    void drawFrameActions(UIObject* object) {
        if (object->enabled == false || object->renderer->FBOwner != nullptr) return;

        object->onFrame();

        for (unsigned int i = 0; i < object->getBehaviours().size(); i++) {
            object->getBehaviours()[i]->update();
        }

        object->renderer->renderObject();

        for (unsigned int i = 0; i < object->getChildren().size(); i++) {
            object->getChildren()[i]->renderer->updateParentTransform(object->renderer->getTransformMatrix());
            drawFrameActions(object->getChildren()[i]);
        }
    }

    void drawTextureObject(UIObject* object) {
        if (object->enabled == false) return;
        
        object->renderer->renderObject();
        
        for (unsigned int i = 0; i < object->getChildren().size(); i++) {
            object->getChildren()[i]->renderer->updateParentTransform(object->renderer->getTransformMatrix());
            drawTextureObject(object->getChildren()[i]);
        }
    }

    void Behaviour::attachToObject(UIObject* obj) {
        object = obj;
    }

    void UIObject::removeChild(int ID) {
        for (size_t i = 0; i < children.size(); i++) {
            if (children[i]->objectID == ID) {
                children.erase(children.begin() + i);
                return;
            }
        }
    }

    void UIObject::addBehaviour(Behaviour* behaviour) {
        behaviours.push_back(behaviour);
        behaviour->attachToObject(this);
    }

    void UIObject::setName(std::string name) {
        if (name == "") {
            if (objectNameMap.find(this->name) != objectNameMap.end()) objectNameMap.erase(this->name);
            this->name = "";
            return;
        }
        if (objectNameMap.find(this->name) != objectNameMap.end()) objectNameMap.erase(this->name);
        if (objectNameMap.find(name) != objectNameMap.end()) objectNameMap[name]->setName("");
        this->name = name;
        objectNameMap[name] = this;
        return;
    }

    std::string UIObject::getName() {
        return name;
    }

    UIObject* Arriba::UIObject::getParent() {
        return parent;
    }

    std::vector<Arriba::UIObject*> UIObject::getChildren() {
        return children;
    }

    std::vector<Arriba::Behaviour*> UIObject::getBehaviours() {
        return behaviours;
    }

    void UIObject::setColour(Arriba::Maths::vec4<float> _colour) {
        renderer->setColour(_colour);
    }

    Arriba::Maths::vec4<float> UIObject::getColour() {
        return renderer->getColour();
    }

    Arriba::Maths::mat4<float> UIObject::getGlobalPos() {
        return renderer->getTransformMatrix();
    }

    void UIObject::setFBOwner(Arriba::Graphics::AdvancedTexture* fb) {
        this->renderer->FBOwner = fb;
        for (unsigned int i = 0; i < children.size(); i++) {
            children[i]->setFBOwner(fb);
        }
    }

    void UIObject::destroy() {
        setParent(nullptr);
        while (getChildren().size() != 0) getChildren()[0]->destroy();
        setName("");
        pendingDestroySet.insert(this);
    }

    void UIObject::setParent(Arriba::UIObject* newParent) {
        if (parent) parent->removeChild(objectID);
        
        if (newParent) {
            newParent->children.push_back(this);
            setFBOwner(newParent->renderer->FBOwner);
        } else {
            setFBOwner(nullptr);
        }

        parent = newParent;
    }

    UIObject::UIObject() {
        objectList.push_back(std::unique_ptr<UIObject>(this));
        objectID = rand();
        renderer = std::make_unique<Arriba::Graphics::Renderer>();
        renderer->transform = &transform;
    }

    UIObject::~UIObject() {
        // renderer.release();
    }

    UIObject* findObjectByName(std::string name) {
        if (objectNameMap.find(name) != objectNameMap.end()) return objectNameMap[name];
        return nullptr;
    }

    std::vector<UIObject*> findObjectsByTag(std::string tag) {
        std::vector<UIObject*> list = {};
        for (size_t i = 0; i < objectList.size(); i++) {
            if (objectList[i]->tag == tag && !pendingDestroySet.count(objectList[i].get())) list.push_back(objectList[i].get());
        }
        return list;
    }
}  // namespace Arriba
