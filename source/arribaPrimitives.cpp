#include <arribaPrimitives.h>
#include <memory>
#include <stdio.h>
#include <cstring>
#include <ft2build.h>
#include FT_FREETYPE_H

Arriba::Primitives::Quad::Quad(int _x, int _y, int _width, int _height, Arriba::Graphics::Pivot pivotMode)
{
    this->transform.position.x = _x;
    this->transform.position.y = _y;
    if((_width != 0) && (_height != 0)) setDimensions(_width, _height, pivotMode);
}

void Arriba::Primitives::Quad::setDimensions(int _width, int _height, Arriba::Graphics::Pivot pivotMode)
{
    width = _width;
    height = _height;

    if(pivotMode == Arriba::Graphics::centre)
    {
        left = -width/2;
        right = width/2;
        top = height/2;
        bottom = -height/2;
    }
    else if(pivotMode == Arriba::Graphics::topLeft)
    {
        left = 0;
        right = width;
        top = height;
        bottom = 0;
    }
    else if(pivotMode == Arriba::Graphics::bottomLeft)
    {
        left = 0;
        right = width;
        top = 0;
        bottom = -height;
    }
    else if(pivotMode == Arriba::Graphics::topRight)
    {
        left = -width;
        right = 0;
        top = height;
        bottom = 0;
    }
    else if(pivotMode == Arriba::Graphics::bottomRight)
    {
        left = -width;
        right = 0;
        top = 0;
        bottom = -height;
    }

    //Texture Y coordinates here are a bit hacked to work around inversion issue
    float verts[20] = {
        right,  top,    0.0f, 1.0f, 0.0f,  // top right
        right,  bottom, 0.0f, 1.0f, 1.0f,  // bottom right
        left,   bottom, 0.0f, 0.0f, 1.0f,  // bottom left
        left,   top,    0.0f, 0.0f, 0.0f  // top left 
    };

    //float verts[20] = {
    //    right,  top,    0.0f, 1.0f, 1.0f,  // top right
    //    right,  bottom, 0.0f, 1.0f, 0.0f,  // bottom right
    //    left,   bottom, 0.0f, 0.0f, 0.0f,  // bottom left
    //    left,   top,    0.0f, 0.0f, 1.0f  // top left 
    //};

    unsigned int indexes[6] = {
        0, 1, 3, //First triangle
        1, 2, 3 //Second triangle
    };

    renderer->loadVerts(verts, sizeof(verts), indexes, sizeof(indexes));
}

float Arriba::Primitives::Quad::getTop()
{
    return top + getGlobalPos()[3].y;
}

float Arriba::Primitives::Quad::getBottom()
{
    return bottom + getGlobalPos()[3].y;
}

float Arriba::Primitives::Quad::getLeft()
{
    return left + getGlobalPos()[3].x;
}

float Arriba::Primitives::Quad::getRight()
{
    return right + getGlobalPos()[3].x;
}

//Is it better to precalculate the dimensions for every character in the font and then just set the VAO?
Arriba::Primitives::Character::Character(Arriba::Graphics::CharInfo character) : Quad(0,0,0,0,Arriba::Graphics::topLeft)
{
    //Arriba::Graphics::CharInfo character = Arriba::Graphics::getFont(size)[c];
    float charWidth = character.size[0];
    float charHeight = character.size[1];
    float charXOffset = character.bearing[0];
    float charYOffset = character.bearing[1];
    float verts[20] = {
        (float)charWidth + charXOffset, (float)charHeight   - charYOffset, 0.0f, 1.0f, 1.0f,  // top right
        (float)charWidth + charXOffset, (float)0            - charYOffset, 0.0f, 1.0f, 0.0f, // bottom right
        (float)0         + charXOffset, (float)0            - charYOffset, 0.0f, 0.0f, 0.0f, // bottom left
        (float)0         + charXOffset, (float)charHeight   - charYOffset, 0.0f, 0.0f, 1.0f  // top left 
    };
    unsigned int indexes[6] = {
            0, 1, 3, //First triangle
            1, 2, 3 //Second triangle
    };

    renderer->updateVerts(verts, sizeof(verts));
    //renderer->thisShader.updateFragments("romfs:/TextVertex.glsl", "romfs:/TextFragment.glsl");
    renderer->thisShader.setProgID(Arriba::Graphics::textShaderID);
    renderer->setTexture(character.texID);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), 0);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(0);
    glEnableVertexAttribArray(1);

    width = character.size[0];
    height = character.size[1];
}

Arriba::Primitives::Text::Text(const char* text, int size) : Quad(0, 0, 0, 0, Arriba::Graphics::Pivot::centre)
{
    fontSize = size;
    setText(text);
}

void Arriba::Primitives::Text::setText(const char* text)
{
    //Nuke the kids
    std::vector<Arriba::UIObject *> kids = getChildren();
    while (kids.size() != 0)
    {
        kids[0]->destroy();
        kids.erase(kids.begin());
    }
    //Vars for storing geometry info
    int xOffset = 0;
    int maxHeight = 0;
    int minHeight = 0;
    //Cache the char map
    std::map<char, Arriba::Graphics::CharInfo> charMap = Arriba::Graphics::getFont(fontSize);
    //Spawn the chars
    for (unsigned int i = 0; i < strnlen(text, 1024); i++)
    {
        Arriba::Graphics::CharInfo character = charMap[text[i]];
        Arriba::Primitives::Quad* child = new Arriba::Primitives::Character(character);
        child->setParent(this);
        child->transform.position.x = xOffset;
        xOffset += (character.advance >> 6);
        maxHeight = (character.size[1] > maxHeight) ? character.size[1] : maxHeight;
        minHeight = (-character.bearing[1] < minHeight) ? -character.bearing[1] : minHeight;

    }
    //Center the text to the parent
    int xDistance = xOffset / 2;
    int yDistance = maxHeight / 2;
    for (unsigned int i = 0; i < strnlen(text, 1024); i++)
    {
        children.at(i)->transform.position.x -= xDistance;
        children.at(i)->transform.position.y += yDistance;
    }
    //Adjust the parent geometry
    setDimensions(xOffset, maxHeight - minHeight / 2, Arriba::Graphics::Pivot::centre);
    //Make BG invisible
    renderer->setColour({0,0,0,0});
    setColour(fontColour);
}

void Arriba::Primitives::Text::setColour(glm::vec4 _colour)
{
    fontColour = _colour;
    for (unsigned int i = 0; i < children.size(); i++)
    {
        children[i]->setColour(fontColour);
    }
}