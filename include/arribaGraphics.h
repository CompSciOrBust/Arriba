#pragma once
#include <map>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <arribaMaths.h>
#include <ft2build.h>
#include FT_FREETYPE_H
// Includes for HOS
#ifdef __SWITCH__
#include <switch.h>
#endif

namespace Arriba::Graphics {
struct CharInfo;
// Vars
extern bool graphicsAreInitialised;
inline GLFWwindow* window;
inline int windowWidth = 1280, windowHeight = 720;
inline int renderWidth = 1280, renderHeight = 720;
inline bool graphicsAreInitialised = false;
inline Arriba::Maths::mat4<float> clipSpaceMatrix;
inline FT_Library ft;
inline FT_Face face;
inline std::map<int, std::map<char, CharInfo>> charMapMap;
inline unsigned int defaultTexture;
inline unsigned int textShaderID;
inline unsigned int defaultShaderID;
// Functions
void initGraphics();
std::map<char, CharInfo> getFont(int size);
unsigned int loadShader(const char* vertexPath, const char* fragmentPath);
unsigned int bufferTexture_RGBA(unsigned int width, unsigned int height, unsigned char* data);
unsigned int bufferTexture_Red(unsigned int width, unsigned int height, unsigned char* data);
// HOS functions
#ifdef __SWITCH__
void dockStatusCallback(AppletHookType type, void* parameters);
#endif

enum Pivot {
    centre,
    topLeft,
    bottomLeft,
    topRight,
    bottomRight
};

class Shader {
    private:
        bool sharedShader = false;

    public:
        Shader(const char* vertexPath, const char* fragmentPath);
        Shader(unsigned int _ID);
        ~Shader();

        void activate();
        void setFloat1(char* uniformName, float data);
        void setFloat2(char* uniformName, Arriba::Maths::vec2<float> data);
        void setFloat3(char* uniformName, Arriba::Maths::vec3<float> data);
        void setFloat4(char* uniformName, Arriba::Maths::vec4<float> data);
        void updateFragments(const char* vertexPath, const char* fragmentPath);
        void setProgID(unsigned int _ID);
        unsigned int progID;
};

class AdvancedTexture {
    public:
    AdvancedTexture(int _width, int _height);
    ~AdvancedTexture();

    void resize(int _width, int _height);
    virtual void update() {}

    unsigned int texID;
    unsigned int FBO;
    Arriba::Maths::mat4<float> clipSpaceMatrix;
    int tHeight;
    int tWidth;
};

class Renderer {
    protected:
        float verts[20] = {
             0.5f,  0.5f, 0.0f, 1.0f, 1.0f,  // top right
             0.5f, -0.5f, 0.0f, 1.0f, 0.0f,  // bottom right
            -0.5f, -0.5f, 0.0f, 0.0f, 0.0f,  // bottom left
            -0.5f,  0.5f, 0.0f, 0.0f, 1.0f   // top left
        };
        unsigned int indexes[6] = {
            0, 1, 3,  // First triangle
            1, 2, 3  // Second triangle
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
        void updateParentTransform(Arriba::Maths::mat4<float> pt);
        void renderObject();
        void loadVerts(float* _verts, unsigned int vertSize, unsigned int* _indexes, unsigned int indexesSize);
        void updateVerts(float* _verts, unsigned int vertSize);
        void setTexture(unsigned int ID);
        void setColour(Arriba::Maths::vec4<float> _colour);
        Arriba::Maths::vec4<float> getColour();

        Arriba::Maths::Transform *transform;
        Arriba::Graphics::AdvancedTexture* FBOwner = nullptr;
};

struct CharInfo {
    unsigned int texID;
    Arriba::Maths::vec2<int> size;
    Arriba::Maths::vec2<int> bearing;
    unsigned int advance;
};
}  // namespace Arriba::Graphics
