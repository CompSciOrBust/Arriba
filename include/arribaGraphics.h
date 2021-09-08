#pragma once
#include <map>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <arribaMaths.h>
#include <glm/glm.hpp>
#include <ft2build.h>
#include FT_FREETYPE_H
//Includes for HOS
#ifdef __SWITCH__
#include <switch.h>
#endif

namespace Arriba::Graphics
{
    struct CharInfo;
    //Vars
    extern bool graphicsAreInitialised;
    inline GLFWwindow* window;
    inline int windowWidth = 1280, windowHeight = 720;
    inline bool graphicsAreInitialised = false;
    inline glm::mat4 clipSpaceMatrix;
    inline FT_Library ft;
    inline FT_Face face;
    inline std::map<int, std::map<char, CharInfo>> charMapMap;
    inline unsigned int defaultTexture;
    inline unsigned int textShaderID;
    inline unsigned int defaultShaderID;
    //Functions
    void initGraphics();
    std::map<char, CharInfo> getFont(int);
    unsigned int loadShader(const char*, const char*);
    //HOS functions
    #ifdef __SWITCH__
    void dockStatusCallback(AppletHookType type, void* parameters);
    #endif

    enum Pivot
    {
        centre,
        topLeft,
        bottomLeft,
        topRight,
        bottomRight
    };

    class Shader
    {
        private:
            bool sharedShader = false;

        public:
            Shader(const char*, const char*);
            Shader(unsigned int);
            ~Shader();
            
            void activate();
            void setFloat1(char*, float);
            void setFloat2(char*, glm::vec2);
            void setFloat3(char*, glm::vec3);
            void setFloat4(char*, glm::vec4);
            void updateFragments(const char*, const char*);
            void setProgID(unsigned int);
            unsigned int progID;
    };

    class AdvancedTexture
    {
        public:
        AdvancedTexture(int _width, int _height);
        ~AdvancedTexture();
        virtual void update();

        unsigned int texID;
        unsigned int FBO;
        glm::mat4 clipSpaceMatrix;
        int height;
        int width;
    };

    class Renderer
    {
        protected:
            float verts[20] = {
                 0.5f,  0.5f, 0.0f, 1.0f, 1.0f,  // top right
                 0.5f, -0.5f, 0.0f, 1.0f, 0.0f,  // bottom right
                -0.5f, -0.5f, 0.0f, 0.0f, 0.0f,  // bottom left
                -0.5f,  0.5f, 0.0f, 0.0f, 1.0f   // top left 
            };
            unsigned int indexes[6] = {
                0, 1, 3, //First triangle
                1, 2, 3 //Second triangle
            };
            glm::vec4 colour = {1, 1, 1, 1};

            unsigned int VBOID;
            unsigned int VAOID;
            unsigned int EBOID;
            unsigned int texID = defaultTexture;
            glm::mat4 parentTransform = glm::mat4(1.0f);

        public:
            Renderer();
            ~Renderer();
            Shader thisShader = Shader(defaultShaderID);

            glm::mat4 getTransformMatrix();
            void updateParentTransform(glm::mat4);
            void renderObject();
            void loadVerts(float*, unsigned int, unsigned int*, unsigned int);
            void updateVerts(float*, unsigned int);
            void setTexture(unsigned int);
            void setColour(glm::vec4);
            glm::vec4 getColour();

            Arriba::Maths::Transform *transform;
            Arriba::Graphics::AdvancedTexture* FBOwner = nullptr;
    };

    struct CharInfo
    {
        unsigned int texID;
        glm::ivec2 size;
        glm::ivec2 bearing;
        unsigned int advance;
    };
}