#include <arribaPrimitives.h>
#include <memory>
#include <stdio.h>
#include <cstring>
#include <ft2build.h>
#include FT_FREETYPE_H

namespace Arriba::Primitives {
    Quad::Quad(int _x, int _y, int _width, int _height, Arriba::Graphics::Pivot pivotMode) {
        this->transform.position.x = _x;
        this->transform.position.y = _y;
        if ((_width != 0) && (_height != 0)) setDimensions(_width, _height, pivotMode);
    }

    void Quad::setDimensions(int _width, int _height, Arriba::Graphics::Pivot pivotMode) {
        width = _width;
        height = _height;

        if (pivotMode == Arriba::Graphics::centre) {
            left = -width/2;
            right = width/2;
            top = height/2;
            bottom = -height/2;
        } else if (pivotMode == Arriba::Graphics::topLeft) {
            left = 0;
            right = width;
            top = height;
            bottom = 0;
        } else if (pivotMode == Arriba::Graphics::bottomLeft) {
            left = 0;
            right = width;
            top = 0;
            bottom = -height;
        } else if (pivotMode == Arriba::Graphics::topRight) {
            left = -width;
            right = 0;
            top = height;
            bottom = 0;
        } else if (pivotMode == Arriba::Graphics::bottomRight) {
            left = -width;
            right = 0;
            top = 0;
            bottom = -height;
        }

        // Texture Y coordinates here are a bit hacked to work around inversion issue
        // float verts[20] = {
        //     right,  top,    0.0f, 1.0f, 0.0f,  // top right
        //     right,  bottom, 0.0f, 1.0f, 1.0f,  // bottom right
        //     left,   bottom, 0.0f, 0.0f, 1.0f,  // bottom left
        //     left,   top,    0.0f, 0.0f, 0.0f  // top left
        // };

        float verts[20] = {
            right,  top,    0.0f, 1.0f, 1.0f,  // top right
            right,  bottom, 0.0f, 1.0f, 0.0f,  // bottom right
            left,   bottom, 0.0f, 0.0f, 0.0f,  // bottom left
            left,   top,    0.0f, 0.0f, 1.0f  // top left
        };

        unsigned int indexes[6] = {
            0, 1, 3,  // First triangle
            1, 2, 3  // Second triangle
        };

        renderer->loadVerts(verts, sizeof(verts), indexes, sizeof(indexes));
    }

    float Quad::getTop() {
        return top + getGlobalPos().col4.y;
    }

    float Quad::getBottom() {
        return bottom + getGlobalPos().col4.y;
    }

    float Quad::getLeft() {
        return left + getGlobalPos().col4.x;
    }

    float Quad::getRight() {
        return right + getGlobalPos().col4.x;
    }

    // Is it better to precalculate the dimensions for every character in the font and then just set the VAO?
    Character::Character(Arriba::Graphics::CharInfo character) : Quad(0, 0, 0, 0, Arriba::Graphics::topLeft) {
        // Arriba::Graphics::CharInfo character = Arriba::Graphics::getFont(size)[c];
        float charWidth = character.size.x;
        float charHeight = character.size.y;
        float charXOffset = character.bearing.x;
        float charYOffset = character.bearing.y;
        float verts[20] = {
            static_cast<float>(charWidth), static_cast<float>(charHeight)   - charYOffset, 0.0f, 1.0f, 1.0f,  // top right
            static_cast<float>(charWidth), static_cast<float>(0)            - charYOffset, 0.0f, 1.0f, 0.0f,  // bottom right
            static_cast<float>(0)        , static_cast<float>(0)            - charYOffset, 0.0f, 0.0f, 0.0f,  // bottom left
            static_cast<float>(0)        , static_cast<float>(charHeight)   - charYOffset, 0.0f, 0.0f, 1.0f   // top left
        };
        unsigned int indexes[6] = {
                0, 1, 3,  // First triangle
                1, 2, 3  // Second triangle
        };

        renderer->updateVerts(verts, sizeof(verts));
        // renderer->thisShader.updateFragments("romfs:/TextVertex.glsl", "romfs:/TextFragment.glsl");
        renderer->thisShader.setProgID(Arriba::Graphics::textShaderID);
        renderer->setTexture(character.texID);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), 0);
        glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
        glEnableVertexAttribArray(0);
        glEnableVertexAttribArray(1);

        width = character.size.x;
        height = character.size.y;
    }

    Text::Text(const char* text, int size) : Arriba::Graphics::AdvancedTexture(1, 1), Quad(0, 0, 0, 0, Arriba::Graphics::Pivot::centre) {
        renderer->setTexture(texID);
        fontSize = size;
        setText(text);
    }

    void Text::setText(const char* text) {
        // Vars for storing geometry info
        int xOffset = 0;
        int maxHeight = 0;
        int minHeight = 0;
        // Cache the char map
        std::map<char, Arriba::Graphics::CharInfo> charMap = Arriba::Graphics::getFont(fontSize);
        // Spawn the chars
        for (unsigned int i = 0; i < strnlen(text, 1024); i++) {
            Arriba::Graphics::CharInfo character = charMap[text[i]];
            Quad* child = new Character(character);
            chars.push_back(child);
            child->setFBOwner(this);
            child->setColour({1, 1, 1, 1});
            child->transform.position.x = xOffset + character.bearing.x;
            xOffset += (character.advance >> 6);
            maxHeight = (character.size.y > maxHeight) ? character.size.y : maxHeight;
            minHeight = (character.size.y - character.bearing.y > minHeight) ? character.size.y - character.bearing.y : minHeight;
        }
        // Center the text to the parent
        int xDistance = 0;
        int yDistance = maxHeight;
        for (unsigned int i = 0; i < strnlen(text, 1024); i++) {
            chars.at(i)->transform.position.x -= xDistance;
            chars.at(i)->transform.position.y += yDistance;
        }
        // Adjust the parent geometry
        setDimensions(xOffset+2, maxHeight + minHeight+2, Arriba::Graphics::Pivot::centre);
        // Set the colour
        setColour(fontColour);
        // Resize the framebuffer
        resize(width, height);
        // Render framebuffer
        update();
    }

    void Text::setColour(Arriba::Maths::vec4<float> _colour) {
        fontColour = _colour;
        renderer->setColour(_colour);
    }

    void Text::update() {
        // Render framebuffer
        glBindFramebuffer(GL_FRAMEBUFFER, FBO);
        glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
        glClear(GL_COLOR_BUFFER_BIT);
        for (unsigned int i = 0; i < chars.size(); i++) {
            drawTextureObject(chars[i]);
        }
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        // Destroy chars to keep FPS high
        while (chars.size() != 0) {
            chars[0]->destroy();
            chars.erase(chars.begin());
        }
    }
}  // namespace Arriba::Primitives
