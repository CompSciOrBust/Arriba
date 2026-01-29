#include <Primitives/character.h>

namespace Arriba::Primitives {
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
} // Arriba::Primitives
