#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <arribaGraphics.h>
#include <arribaMaths.h>
#include <stdio.h>
#include <string>
#include <iostream>
#include <fstream>

// True type
#include <ft2build.h>
// Libnx
#include <switch.h>

namespace Arriba::Graphics {
    void initGraphics() {
        graphicsAreInitialised = true;
        // GLFW init
        glfwInit();
        glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
        glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
        // Make window
        window = glfwCreateWindow(windowWidth, windowHeight, "Arriba", NULL, NULL);
        if (window == NULL) printf("Failed to create window\n");
        glfwMakeContextCurrent(window);
        // Init glad
        if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) printf("Glad init failed\n");
        // Set the view port size and position
        glViewport(0, 0, renderWidth, renderHeight);
        // Create the clip space matrix
        clipSpaceMatrix = Arriba::Maths::makeOrtho(0.0f, static_cast<float>(windowWidth), static_cast<float>(windowHeight), 0.0f, 0.0f, 10000.0f);
        // Enable blending
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        // Generate a blank texture for objects use
        unsigned char textureData[] = {255, 255, 255, 255};
        defaultTexture = bufferTexture_RGBA(1, 1, textureData);
        // Load text shader
        textShaderID = loadShader("romfs:/TextVertex.glsl", "romfs:/TextFragment.glsl");
        defaultShaderID = loadShader("romfs:/VertexDefault.glsl", "romfs:/FragmentDefault.glsl");
    }

    // Get ready for some cursed shit
    std::map<char, CharInfo> getFont(int size) {
        // Check if the font is loaded
        std::map<int, std::map<char, CharInfo>>::iterator mapIt = charMapMap.find(size);
        // It is so return it
        if (mapIt != charMapMap.end()) {
            return mapIt->second;
        } else {  // Font is not loaded, so load it
            std::map<char, CharInfo> charMapT;
            // Init true type
            if (FT_Init_FreeType(&ft)) printf("Failed to init free type\n");
            // Init pl service
            plInitialize(PlServiceType_User);
            // Load font for debugging
            PlFontData standardFontData;
            plGetSharedFontByType(&standardFontData, PlSharedFontType_Standard);
            FT_New_Memory_Face(ft, reinterpret_cast<FT_Byte*>(standardFontData.address), standardFontData.size, 0, &face);
            FT_Set_Pixel_Sizes(face, 0, size);
            glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
            for (unsigned char c = 0; c < 255; c++) {
                // Load the glyph
                if (FT_Load_Char(face, FT_Get_Char_Index(face, c), FT_LOAD_RENDER)) printf("Failed to load char\n");
                // Gen the texture
                unsigned int texture = bufferTexture_Red(face->glyph->bitmap.width, face->glyph->bitmap.rows, face->glyph->bitmap.buffer);
                // glGenTextures(1, &texture);
                // glBindTexture(GL_TEXTURE_2D, texture);
                // glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, face->glyph->bitmap.width, face->glyph->bitmap.rows, 0, GL_RED, GL_UNSIGNED_BYTE, face->glyph->bitmap.buffer);
                // glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
                // glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
                // glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
                // glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
                CharInfo character = {
                    texture,
                    Arriba::Maths::vec2<int>{face->glyph->bitmap.width, face->glyph->bitmap.rows},
                    Arriba::Maths::vec2<int>{face->glyph->bitmap_left, face->glyph->bitmap_top},
                    static_cast<unsigned int>(face->glyph->advance.x)};
                charMapT.insert(std::make_pair(FT_Get_Char_Index(face, c), character));
            }
            // Clean up freetype
            FT_Done_Face(face);
            FT_Done_FreeType(ft);
            // Exit PL
            plExit();
            charMapMap.insert(std::make_pair(size, charMapT));
            return charMapT;
        }
    }

    unsigned int loadShader(const char* vertexPath, const char* fragmentPath) {
        // vars
        std::ifstream fileStream(vertexPath);
        std::string vertexCode;
        std::string fragmentCode;
        std::string codeLine;
        unsigned int SID;
        // Read the data
        while (!fileStream.eof()) {
            getline(fileStream, codeLine);
            vertexCode += codeLine + "\n";
        }
        fileStream.close();
        fileStream.open(fragmentPath);
        while (!fileStream.eof()) {
            getline(fileStream, codeLine);
            fragmentCode += codeLine + "\n";
        }
        // Compile vertex shader
        unsigned int vertex, fragment;
        int success;
        char debugInfo[512];
        vertex = glCreateShader(GL_VERTEX_SHADER);
        const char* vertexCodeChars = vertexCode.c_str();
        glShaderSource(vertex, 1, &vertexCodeChars, NULL);
        glCompileShader(vertex);
        // Compile fragment shader
        fragment = glCreateShader(GL_FRAGMENT_SHADER);
        const char* fragmentCodeChars = fragmentCode.c_str();
        glShaderSource(fragment, 1, &fragmentCodeChars, NULL);
        glCompileShader(fragment);
        // Get error info
        glGetShaderiv(vertex, GL_COMPILE_STATUS, &success);
        if (!success) {
            glGetShaderInfoLog(vertex, 512, NULL, debugInfo);
            printf("%s\n", debugInfo);
        }
        glGetShaderiv(fragment, GL_COMPILE_STATUS, &success);
        if (!success) {
            glGetShaderInfoLog(fragment, 512, NULL, debugInfo);
            printf("%s\n", debugInfo);
        }
        // Link the shaders
        SID = glCreateProgram();
        glAttachShader(SID, vertex);
        glAttachShader(SID, fragment);
        glLinkProgram(SID);
        // Get error info
        glGetProgramiv(SID, GL_LINK_STATUS, &success);
        if (!success) {
            glGetProgramInfoLog(SID, 512, NULL, debugInfo);
            printf("%s\n", debugInfo);
        }
        // Clean up
        glDeleteShader(vertex);
        glDeleteShader(fragment);
        return SID;
    }

    unsigned int bufferTexture_RGBA(unsigned int width, unsigned int height, unsigned char* data) {
        unsigned int textureID = 0;
        // Generate a texture
        glGenTextures(1, &textureID);
        glBindTexture(GL_TEXTURE_2D, textureID);
        // Buffer the data
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        // Unbind the texture and return
        glBindTexture(GL_TEXTURE_2D, 0);
        return textureID;
    }

    unsigned int bufferTexture_Red(unsigned int width, unsigned int height, unsigned char* data) {
        unsigned int textureID = 0;
        // Generate a texture
        glGenTextures(1, &textureID);
        glBindTexture(GL_TEXTURE_2D, textureID);
        // Buffer the data
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, width, height, 0, GL_RED, GL_UNSIGNED_BYTE, data);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        // Unbind the texture and return
        glBindTexture(GL_TEXTURE_2D, 0);
        return textureID;
    }

    // HOS Functions
    #ifdef __SWITCH__
    void dockStatusCallback(AppletHookType type, void* parameters) {
        switch (appletGetOperationMode()) {
            case AppletOperationMode_Handheld:
            renderWidth = 1280;
            renderHeight = 720;
            break;
            case AppletOperationMode_Console:
            renderWidth = 1920;
            renderHeight = 1080;
            break;
        }
        glViewport(0, 0, renderWidth, renderHeight);
    }
    #endif

    Shader::Shader(const char* vertexPath = "romfs:/VertexDefault.glsl", const char* fragmentPath = "romfs:/FragmentDefault.glsl") {
        updateFragments(vertexPath, fragmentPath);
    }

    Shader::Shader(unsigned int _ID) {
        sharedShader = true;
        progID = _ID;
    }

    Shader::~Shader() {
        if (!sharedShader) glDeleteProgram(progID);
    }

    void Shader::activate() {
        glUseProgram(progID);
    }

    void Shader::setFloat1(char* uniformName, float data) {
        activate();
        glUniform1f(glGetUniformLocation(progID, uniformName), data);
    }

    void Shader::setFloat2(char* uniformName, Arriba::Maths::vec2<float> data) {
        activate();
        glUniform2f(glGetUniformLocation(progID, uniformName), data.x, data.y);
    }

    void Shader::setFloat3(char* uniformName, Arriba::Maths::vec3<float> data) {
        activate();
        glUniform3f(glGetUniformLocation(progID, uniformName), data.x, data.y, data.z);
    }

    void Shader::setFloat4(char* uniformName, Arriba::Maths::vec4<float> data) {
        activate();
        glUniform4f(glGetUniformLocation(progID, uniformName), data.x, data.y, data.z, data.w);
    }

    void Shader::updateFragments(const char* vertexPath, const char* fragmentPath) {
        // Delete old shader
        if (!sharedShader) glDeleteProgram(progID);
        progID = loadShader(vertexPath, fragmentPath);
    }

    void Shader::setProgID(unsigned int _ID) {
        if (!sharedShader) glDeleteProgram(progID);
        progID = _ID;
        sharedShader = true;
    }

    AdvancedTexture::AdvancedTexture(int _width, int _height) {
        tWidth = _width;
        tHeight = _height;
        // Gen and bind the new frame buffer
        glGenFramebuffers(1, &FBO);
        glBindFramebuffer(GL_FRAMEBUFFER, FBO);
        // Generate a new texture to target from the frame buffer
        glGenTextures(1, &texID);
        glBindTexture(GL_TEXTURE_2D, texID);
        // Note we don't write data to the texture since we're rendering to it from the frame buffer
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, tWidth, _height, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        // Attach the texture to the frame buffer
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, texID, 0);
        if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) printf("Error: framebuffer incomplete\n");
        // Unbind the frame buffer
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        glBindTexture(GL_TEXTURE_2D, 0);
        // Create clipspace for framebuffer
        clipSpaceMatrix = Arriba::Maths::makeOrtho(0.0f, static_cast<float>(tWidth), 0.0f, static_cast<float>(tHeight), 0.0f, 10000.0f);
    }

    AdvancedTexture::~AdvancedTexture() {
        // Delete the frame buffer
        glDeleteFramebuffers(1, &FBO);
        // Delete the texture
        glDeleteTextures(1, &texID);
    }

    void AdvancedTexture::resize(int _width, int _height) {
        tWidth = _width;
        tHeight = _height;
        glBindTexture(GL_TEXTURE_2D, texID);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, tWidth, tHeight, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
        glBindTexture(GL_TEXTURE_2D, 0);
        clipSpaceMatrix = Arriba::Maths::makeOrtho(0.0f, static_cast<float>(tWidth), 0.0f, static_cast<float>(tHeight), 0.0f, 10000.0f);
    }

    Renderer::Renderer() {
        // Load shader
        // thisShader = Shader();
        // Create a vertex array object
        glGenVertexArrays(1, &VAOID);
        glBindVertexArray(VAOID);
        // Generate a buffer for the verts
        glGenBuffers(1, &VBOID);
        glBindBuffer(GL_ARRAY_BUFFER, VBOID);
        glBufferData(GL_ARRAY_BUFFER, sizeof(verts), verts, GL_DYNAMIC_DRAW);
        // Create the EBO to store the vert indexes
        glGenBuffers(1, &EBOID);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBOID);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indexes), indexes, GL_DYNAMIC_DRAW);
        // Specify the vertex attributes
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
        glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
        glEnableVertexAttribArray(0);
        glEnableVertexAttribArray(1);

        // Unbind the vertex array
        glBindVertexArray(0);
        // Unbind the buffer object
        // glBindBuffer(GL_ARRAY_BUFFER, 0);
        // Unbind the EBO
        // glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
    }

    Renderer::~Renderer() {
        glDeleteVertexArrays(1, &VAOID);
        glDeleteBuffers(1, &VBOID);
        glDeleteBuffers(1, &EBOID);
    }

    Arriba::Maths::mat4<float> Renderer::getTransformMatrix() {
        Arriba::Maths::mat4<float> transformationMatrix;
        transformationMatrix = Arriba::Maths::translate(transformationMatrix, Arriba::Maths::vec3<float>{transform->position.x, transform->position.y, transform->position.z});
        transformationMatrix = Arriba::Maths::rotate(transformationMatrix, Arriba::Maths::radians(transform->rotation.x), Arriba::Maths::vec3<float>{1.0f, 0.0f, 0.0f});
        transformationMatrix = Arriba::Maths::rotate(transformationMatrix, Arriba::Maths::radians(transform->rotation.y), Arriba::Maths::vec3<float>{0.0f, 1.0f, 0.0f});
        transformationMatrix = Arriba::Maths::rotate(transformationMatrix, Arriba::Maths::radians(transform->rotation.z), Arriba::Maths::vec3<float>{0.0f, 0.0f, 1.0f});
        transformationMatrix = Arriba::Maths::scale(transformationMatrix, Arriba::Maths::vec3<float>{transform->scale.x, transform->scale.y, transform->scale.z});
        return parentTransform * transformationMatrix;
    }

    void Renderer::updateParentTransform(Arriba::Maths::mat4<float> pt) {
        parentTransform = pt;
    }

    void Renderer::renderObject() {
        // Activate shader, set the colour, and bind the VAO
        thisShader.activate();
        thisShader.setFloat4("colour", colour);
        glBindVertexArray(VAOID);

        // Tranform the verts to the correct location
        Arriba::Maths::mat4 transformationMatrix = getTransformMatrix();
        unsigned int transformLoc = glGetUniformLocation(thisShader.progID, "transform");
        // TODO: Make transform matrix upload as an actual float array, this hack may not work on other platforms
        glUniformMatrix4fv(transformLoc, 1, GL_FALSE, (GLfloat*)&(transformationMatrix));
        // Get the projection matrix
        unsigned int projectionLoc = glGetUniformLocation(thisShader.progID, "projection");
        // Bind any textures
        glBindTexture(GL_TEXTURE_2D, texID);
        // If belongs to a framebuffer
        if (FBOwner != nullptr) {
            // Bind the FB and set the view port size
            glBindFramebuffer(GL_FRAMEBUFFER, FBOwner->FBO);
            glViewport(0, 0, FBOwner->tWidth, FBOwner->tHeight);
            // Set the transform matrix to the FB's clipspace matrix
            // TODO: Make clip space matrix upload as an actual float array, this hack may not work on other platforms
            glUniformMatrix4fv(projectionLoc, 1, GL_FALSE, (GLfloat*)&(FBOwner->clipSpaceMatrix));
            // Draw object and undo changes to OpenGL's state
            glDrawElements(GL_TRIANGLES, sizeof(indexes), GL_UNSIGNED_INT, 0);
            glBindFramebuffer(GL_FRAMEBUFFER, 0);
            glViewport(0, 0, renderWidth, renderHeight);
        } else {  // If not beloning to an FBO render as normal
            // TODO: Make clip space matrix upload as an actual float array, this hack may not work on other platforms
            glUniformMatrix4fv(projectionLoc, 1, GL_FALSE, (GLfloat*)&(clipSpaceMatrix));
            glDrawElements(GL_TRIANGLES, sizeof(indexes), GL_UNSIGNED_INT, 0);
        }
        glBindVertexArray(0);
    }

    void Renderer::loadVerts(float* _verts, unsigned int vertSize, unsigned int* _indexes, unsigned int indexesSize) {
        // Bind the vao
        glBindVertexArray(VAOID);
        // Buffer the data in to the VBO
        glBindBuffer(GL_ARRAY_BUFFER, VBOID);
        glBufferData(GL_ARRAY_BUFFER, vertSize, _verts, GL_DYNAMIC_DRAW);
        // Buffer the indexes in to the EBO
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBOID);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, indexesSize, _indexes, GL_DYNAMIC_DRAW);
    }

    void Renderer::updateVerts(float* _verts, unsigned int vertSize) {
        // Bind the VAO
        glBindVertexArray(VAOID);
        // Sub buffer the new verts in to the VBO
        glBufferSubData(GL_ARRAY_BUFFER, 0, vertSize, _verts);
    }

    void Renderer::setTexture(unsigned int ID) {
        glActiveTexture(GL_TEXTURE0);
        texID = ID;
    }

    void Renderer::setColour(Arriba::Maths::vec4<float> _colour) {
        colour = _colour;
    }

    Arriba::Maths::vec4<float> Renderer::getColour() {
        return colour;
    }
}  // namespace Arriba::Graphics
