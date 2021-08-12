#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <arribaGraphics.h>
#include <stdio.h>
#include <string>
#include <iostream>
#include <fstream>

//GLM
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
//True type
#include <ft2build.h>
//Libnx
#include <switch.h>

void Arriba::Graphics::initGraphics()
{
    graphicsAreInitialised = true;
    //GLFW init
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    //Make window
    window = glfwCreateWindow(windowWidth, windowHeight, "Arriba", NULL, NULL);
    if(window == NULL) printf("Failed to create window\n");
    glfwMakeContextCurrent(window);
    //Init glad
    if(!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) printf("Glad init failed\n");
    //Set the view port size and position
    glViewport(0, 0, windowWidth, windowHeight);
    //Create the clip space matrix
    clipSpaceMatrix = glm::ortho(0.0f, (float)windowWidth, float(windowHeight), 0.0f, 0.0f, 10000.0f);
    //Enable blending
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    //Generate a blank texture for object use
    char textureData[] = {255,255,255,255};
    glGenTextures(1, &defaultTexture);
    glBindTexture(GL_TEXTURE_2D, defaultTexture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, 1, 1, 0, GL_RGBA, GL_UNSIGNED_BYTE, textureData);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glBindTexture(GL_TEXTURE_2D, 0);
    //Load text shader
    textShaderID = loadShader("romfs:/TextVertex.glsl", "romfs:/TextFragment.glsl");
    defaultShaderID = loadShader("romfs:/VertexDefault.glsl","romfs:/FragmentDefault.glsl");
}

//Get ready for some cursed shit
std::map<char, Arriba::Graphics::CharInfo> Arriba::Graphics::getFont(int size)
{
    //Check if the font is loaded
    std::map<int, std::map<char, CharInfo>>::iterator mapIt = Arriba::Graphics::charMapMap.find(size);
    //It is so return it
    if(mapIt != Arriba::Graphics::charMapMap.end())
    {
        return mapIt->second;
    }
    //Font is not loaded to load it
    else
    {
        std::map<char, CharInfo> charMapT;
        //Init true type
        if(FT_Init_FreeType(&ft)) printf("Failed to init free type\n");
        //Init pl service
        plInitialize(PlServiceType_User);
        //Load font for debugging
        PlFontData standardFontData;
        plGetSharedFontByType(&standardFontData, PlSharedFontType_Standard);
        FT_New_Memory_Face(ft, (FT_Byte*)standardFontData.address, standardFontData.size, 0, &face);
        FT_Set_Pixel_Sizes(face, 0, size);
        glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
        for (unsigned char c = 0; c < 128; c++)
        {
            //Load the glyph
            if(FT_Load_Char(face, c, FT_LOAD_RENDER)) printf("Failed to load char\n");
            //Gen the texture
            unsigned int texture;
            glGenTextures(1, &texture);
            glBindTexture(GL_TEXTURE_2D, texture);
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, face->glyph->bitmap.width, face->glyph->bitmap.rows, 0, GL_RED, GL_UNSIGNED_BYTE, face->glyph->bitmap.buffer);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
            CharInfo character = {
                texture,
                glm::ivec2(face->glyph->bitmap.width, face->glyph->bitmap.rows),
                glm::ivec2(face->glyph->bitmap_left, face->glyph->bitmap_top),
                static_cast<unsigned int>(face->glyph->advance.x)};
            charMapT.insert(std::make_pair(c, character));
        }
        //Clean up freetype
        FT_Done_Face(face);
        FT_Done_FreeType(ft);
        //Exit PL
        plExit();
        charMapMap.insert(std::make_pair(size, charMapT));
        return charMapT;
    }
}

unsigned int Arriba::Graphics::loadShader(const char* vertexPath, const char* fragmentPath)
{
    //vars
    std::ifstream fileStream(vertexPath);
    std::string vertexCode;
    std::string fragmentCode;
    std::string codeLine;
    unsigned int SID;
    //Read the data
    while(!fileStream.eof())
    {
        getline(fileStream, codeLine);
        vertexCode += codeLine + "\n";
    }
    fileStream.close();
    fileStream.open(fragmentPath);
    while(!fileStream.eof())
    {
        getline(fileStream, codeLine);
        fragmentCode += codeLine + "\n";
    }
    //Compile vertex shader
    unsigned int vertex, fragment;
    int success;
    char debugInfo[512];
    vertex = glCreateShader(GL_VERTEX_SHADER);
    const char* vertexCodeChars = vertexCode.c_str();
    glShaderSource(vertex, 1, &vertexCodeChars, NULL);
    glCompileShader(vertex);
    //Compile fragment shader
    fragment = glCreateShader(GL_FRAGMENT_SHADER);
    const char* fragmentCodeChars = fragmentCode.c_str();
    glShaderSource(fragment, 1, &fragmentCodeChars, NULL);
    glCompileShader(fragment);
    //Get error info
    glGetShaderiv(vertex, GL_COMPILE_STATUS, &success);
    if(!success)
    {
        glGetShaderInfoLog(vertex, 512, NULL, debugInfo);
        printf("%s\n", debugInfo);
    }
    glGetShaderiv(fragment, GL_COMPILE_STATUS, &success);
    if(!success)
    {
        glGetShaderInfoLog(fragment, 512, NULL, debugInfo);
        printf("%s\n", debugInfo);
    }
    //Link the shaders
    SID = glCreateProgram();
    glAttachShader(SID, vertex);
    glAttachShader(SID, fragment);
    glLinkProgram(SID);
    //Get error info
    glGetProgramiv(SID, GL_LINK_STATUS, &success);
    if(!success)
    {
        glGetProgramInfoLog(SID, 512, NULL, debugInfo);
        printf("%s\n", debugInfo);
    }
    //Clean up
    glDeleteShader(vertex);
    glDeleteShader(fragment);
    return SID;
}

Arriba::Graphics::Shader::Shader(const char* vertexPath = "romfs:/VertexDefault.glsl", const char* fragmentPath = "romfs:/FragmentDefault.glsl")
{
    updateFragments(vertexPath, fragmentPath);
}

Arriba::Graphics::Shader::Shader(unsigned int _ID)
{
    sharedShader = true;
    progID = _ID;
}

Arriba::Graphics::Shader::~Shader()
{
    if(!sharedShader) glDeleteProgram(progID);
}

void Arriba::Graphics::Shader::activate()
{
    glUseProgram(progID);
}

void Arriba::Graphics::Shader::setFloat3(char* uniformName, glm::vec3 data)
{
    activate();
    glUniform3f(glGetUniformLocation(progID, uniformName), data.x, data.y, data.z);
}

void Arriba::Graphics::Shader::setFloat4(char* uniformName, glm::vec4 data)
{
    activate();
    glUniform4f(glGetUniformLocation(progID, uniformName), data.x, data.y, data.z, data.w);
}

void Arriba::Graphics::Shader::updateFragments(const char* vertexPath, const char* fragmentPath)
{
    //Delete old shader
    if(!sharedShader) glDeleteProgram(progID);
    progID = loadShader(vertexPath, fragmentPath);
}

void Arriba::Graphics::Shader::setProgID(unsigned int _ID)
{
    if(!sharedShader) glDeleteProgram(progID);
    progID = _ID;
    sharedShader = true;
}

Arriba::Graphics::Renderer::Renderer()
{
    //Load shader
    //thisShader = Shader();
    //Create a vertex array object
    glGenVertexArrays(1, &VAOID);
    glBindVertexArray(VAOID);
    //Generate a buffer for the verts
    glGenBuffers(1, &VBOID);
    glBindBuffer(GL_ARRAY_BUFFER, VBOID);
    glBufferData(GL_ARRAY_BUFFER, sizeof(verts), verts, GL_DYNAMIC_DRAW);
    //Create the EBO to store the vert indexes
    glGenBuffers(1, &EBOID);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBOID);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indexes), indexes, GL_DYNAMIC_DRAW);
    //Specify the vertex attributes
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3*sizeof(float), (void*)0);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(0);
    glEnableVertexAttribArray(1);

    //Unbind the vertex array
    glBindVertexArray(0);
    //Unbind the buffer object
    //glBindBuffer(GL_ARRAY_BUFFER, 0);
    //Unbind the EBO
    //glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

Arriba::Graphics::Renderer::~Renderer()
{
    glDeleteVertexArrays(1, &VAOID);
    glDeleteBuffers(1, &VBOID);
    glDeleteBuffers(1, &EBOID);
}

glm::mat4 Arriba::Graphics::Renderer::getTransformMatrix()
{
    glm::mat4 transformationMatrix = glm::mat4(1.0f);
    transformationMatrix = glm::translate(transformationMatrix, glm::vec3(transform->position.x, transform->position.y, transform->position.z));
    transformationMatrix = glm::rotate(transformationMatrix, glm::radians(transform->rotation.x), glm::vec3(1.0f,0.0f,0.0f));
    transformationMatrix = glm::rotate(transformationMatrix, glm::radians(transform->rotation.y), glm::vec3(0.0f,1.0f,0.0f));
    transformationMatrix = glm::rotate(transformationMatrix, glm::radians(transform->rotation.z), glm::vec3(0.0f,0.0f,1.0f));
    transformationMatrix = glm::scale(transformationMatrix, glm::vec3(transform->scale.x, transform->scale.y, transform->scale.z));
    return parentTransform * transformationMatrix;
}

void Arriba::Graphics::Renderer::updateParentTransform(glm::mat4 pt)
{
    parentTransform = pt;
}

void Arriba::Graphics::Renderer::renderObject()
{
    thisShader.activate();
    thisShader.setFloat4("colour", colour);
    glBindVertexArray(VAOID);

    //Tranform the verts to the correct location
    glm::mat4 transformationMatrix = getTransformMatrix();

    unsigned int transformLoc = glGetUniformLocation(thisShader.progID, "transform");
    glUniformMatrix4fv(transformLoc, 1, GL_FALSE, glm::value_ptr(transformationMatrix));
    unsigned int projectionLoc = glGetUniformLocation(thisShader.progID, "projection");
    glUniformMatrix4fv(projectionLoc, 1, GL_FALSE, glm::value_ptr(clipSpaceMatrix));

    glBindTexture(GL_TEXTURE_2D, texID);
    glDrawElements(GL_TRIANGLES, sizeof(indexes), GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);
}

void Arriba::Graphics::Renderer::loadVerts(float* _verts, unsigned int vertSize, unsigned int* _indexes, unsigned int indexesSize)
{
    //Bind the vao
    glBindVertexArray(VAOID);
    //Buffer the data in to the VBO
    glBindBuffer(GL_ARRAY_BUFFER, VBOID);
    glBufferData(GL_ARRAY_BUFFER, vertSize, _verts, GL_DYNAMIC_DRAW);
    //Buffer the indexes in to the EBO
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBOID);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indexesSize, _indexes, GL_DYNAMIC_DRAW);
}

void Arriba::Graphics::Renderer::updateVerts(float* _verts, unsigned int vertSize)
{
    //Bind the VAO
    glBindVertexArray(VAOID);
    //Sub buffer the new verts in to the VBO
    glBufferSubData(GL_ARRAY_BUFFER, 0, vertSize, _verts);
}

void Arriba::Graphics::Renderer::setTexture(unsigned int ID)
{
    glActiveTexture(GL_TEXTURE0);
    texID = ID;
}

void Arriba::Graphics::Renderer::setColour(glm::vec4 _colour)
{
    colour = _colour;
}

glm::vec4 Arriba::Graphics::Renderer::getColour()
{
    return colour;
}

Arriba::Graphics::AdvancedTexture::AdvancedTexture(int _width, int _height)
{
    width = _width;
    height = _height;
    //Gen and bind the new frame buffer
    glGenFramebuffers(1, &FBO);
    glBindFramebuffer(GL_FRAMEBUFFER, FBO);
    //Generate a new texture to target from the frame buffer
    glGenTextures(1, &texID);
    glBindTexture(GL_TEXTURE_2D, texID);
    //Note we don't write data to the texture since we're rendering to it from the frame buffer
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    //Attach the texture to the frame buffer
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, texID, 0);
    if(glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) printf("Error: framebuffer incomplete\n");
    //Unbind the frame buffer
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

Arriba::Graphics::AdvancedTexture::~AdvancedTexture()
{
    //Delete the frame buffer
    glDeleteFramebuffers(1, &FBO);
    //Delete the texture
    glDeleteTextures(1, &texID);
}

//DEBUG ONLY
void Arriba::Graphics::AdvancedTexture::update()
{
    printf("Updating\n");
    glBindFramebuffer(GL_FRAMEBUFFER, FBO);
    glClearColor(0.0f, 0.3f, 0.6f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);
    //Do magic here
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}