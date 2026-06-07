#pragma once

#include <vector>
#include <unordered_map>
#include <unordered_set>
#include <memory>
#include <string>
#include <arribaMaths.h>
#include <arribaGraphics.h>
#include <arribaInput.h>
#include <switch.h>

namespace Arriba {
class UIObject;
inline std::vector<std::shared_ptr<UIObject>> objectList;
inline std::unordered_set<UIObject*> pendingDestroySet;
inline double deltaTime = 0;
inline double time = 0;
inline unsigned long lastFrameTime = 0;
inline UIObject* highlightedObject = nullptr;
inline unsigned int activeLayer = 0;
// variables needed for Horizon OS
#ifdef __SWITCH__
inline AppletHookCookie switchDockCookie;
#endif

class Behaviour {
    protected:
    UIObject* object = nullptr;
    public:
    void attachToObject(UIObject* obj);
    virtual void update() {}
    virtual void onEnable() {}
    virtual void onDisable() {}
};

class UIObject {
    protected:
        UIObject* parent = nullptr;
        std::vector<UIObject*> children;
        std::vector<Behaviour*> behaviours;
        int layer = activeLayer;
        std::string name;
        std::string tag;

    public:
        UIObject();
        virtual ~UIObject();

        void setParent(UIObject* newParent);
        void removeChild(int ID);
        void addBehaviour(Behaviour* behaviour);
        void setName(const std::string& name);
        void setTag(const std::string& tag);
        std::string getName();
        std::string getTag();
        UIObject* getParent();
        std::vector<UIObject*> getChildren();
        std::vector<Behaviour*> getBehaviours();
        virtual void onFrame() {}
        virtual void setColour(const Arriba::Maths::vec4<float>& colour);
        Arriba::Maths::vec4<float> getColour();
        Arriba::Maths::mat4<float> getGlobalPos();
        void setFBOwner(Arriba::Graphics::AdvancedTexture* fb);
        void destroy();

        Arriba::Maths::Transform transform;
        int objectID;
        bool enabled = true;
        std::unique_ptr<Arriba::Graphics::Renderer> renderer;
};

void init();
void exit();
void drawFrame();
void drawFrameActions(UIObject* object);
void drawTextureObject(UIObject* object);
UIObject* findObjectByName(const std::string& name);
std::vector<UIObject*> findObjectsByTag(const std::string& tag);

template<typename T>
T* findObjectByName(const std::string& name) {
    return static_cast<T*>(findObjectByName(name));
}

template<typename T>
std::vector<T*> findObjectsByTag(const std::string& tag) {
    std::vector<UIObject*> base = findObjectsByTag(tag);
    std::vector<T*> result;
    result.reserve(base.size());
    for (UIObject* obj : base) result.push_back(static_cast<T*>(obj));
    return result;
}
}  // namespace Arriba

namespace Arriba::Colour {
    inline Arriba::Maths::vec4<float> neutral = {0.22, 0.47, 0.93, 1};
    inline Arriba::Maths::vec4<float> highlightA = {0.1, 0.95, 0.98, 1};
    inline Arriba::Maths::vec4<float> highlightB = {0.5, 0.85, 1, 1};
    inline Arriba::Maths::vec4<float> activatedColour = {1, 1, 1, 1};
}  // namespace Arriba::Colour
