#pragma once
#include <map>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <arribaMaths.h>
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
    inline Arriba::Maths::mat4<float> clipSpaceMatrix;
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
            void setFloat2(char*, Arriba::Maths::vec2<float>);
            void setFloat3(char*, Arriba::Maths::vec3<float>);
            void setFloat4(char*, Arriba::Maths::vec4<float>);
            void updateFragments(const char*, const char*);
            void setProgID(unsigned int);
            unsigned int progID;
    };

    class AdvancedTexture
    {
        public:
        AdvancedTexture(int _width, int _height);
        ~AdvancedTexture();

        void resize(int _width, int _height);
        virtual void update(){};

        unsigned int texID;
        unsigned int FBO;
        Arriba::Maths::mat4<float> clipSpaceMatrix;
        int tHeight;
        int tWidth;
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
            Arriba::Maths::vec4<float> colour = {1, 1, 1, 1};

            unsigned int VBOID;
            unsigned int VAOID;
            unsigned int EBOID;
            unsigned int texID = defaultTexture;
            Arriba::Maths::mat4<float> parentTransform;

        public:
            Renderer();
            ~Renderer();
            Shader thisShader = Shader(defaultShaderID);

            Arriba::Maths::mat4<float> getTransformMatrix();
            void updateParentTransform(Arriba::Maths::mat4<float>);
            void renderObject();
            void loadVerts(float*, unsigned int, unsigned int*, unsigned int);
            void updateVerts(float*, unsigned int);
            void setTexture(unsigned int);
            void setColour(Arriba::Maths::vec4<float>);
            Arriba::Maths::vec4<float> getColour();

            Arriba::Maths::Transform *transform;
            Arriba::Graphics::AdvancedTexture* FBOwner = nullptr;
    };

    struct CharInfo
    {
        unsigned int texID;
        Arriba::Maths::vec2<int> size;
        Arriba::Maths::vec2<int> bearing;
        unsigned int advance;
    };
}