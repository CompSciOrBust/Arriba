#pragma once
#include <vector>
#include <memory>
#include <string>
#include <arribaMaths.h>
#include <arribaGraphics.h>
#include <arribaInput.h>

namespace Arriba
{
    class UIObject;
    inline std::vector<std::shared_ptr<UIObject>> objectList;
    inline double deltaTime = 0;
    inline unsigned long lastFrameTime = 0;
    inline UIObject* highlightedObject = NULL;

    class Behaviour
    {
        protected:
        UIObject* object;
        public:
        void attachToObject(UIObject*);
        virtual void update(){};
        virtual void onEnable(){};
        virtual void onDisable(){};
    };

    class UIObject
    {
        protected:
            UIObject* parent = nullptr;
            std::vector<UIObject*> children;
            std::vector<Behaviour*> behaviours;

        public:
            UIObject();
            ~UIObject();
            
            void setParent(UIObject* newParent);
            void removeChild(int ID);
            void addBehaviour(Behaviour*);
            UIObject* getParent();
            std::vector<UIObject*> getChildren();
            std::vector<Behaviour*> getBehaviours();
            virtual void onFrame(){};
            virtual void setColour(glm::vec4);
            glm::vec4 getColour();
            glm::mat4 getGlobalPos();
            void destroy();

            Arriba::Maths::Transform transform;
            std::string name;
            std::string tag;
            int objectID;
            bool enabled = true;
            std::unique_ptr<Arriba::Graphics::Renderer> renderer;
    };

    void init();
    void exit();
    void drawFrame();
    void drawFrameActions(UIObject* object);
    UIObject* findObjectByName(std::string name);
    std::vector<UIObject*> findObjectsByTag(std::string tag);
}