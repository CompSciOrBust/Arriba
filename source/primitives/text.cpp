#include <primitives/text.h>
#include <primitives/character.h>
#include <arribaText.h>

namespace Arriba::Primitives {
    Text::Text(const char* text, int size) : Arriba::Graphics::AdvancedTexture(1, 1), Quad(0, 0, 0, 0, Arriba::Graphics::Pivot::centre) {
        renderer->setTexture(texID);
        fontSize = size;
        char32_t* converted = Arriba::Text::ASCIIToUnicode(text);
        setText(converted);
        free(converted);
    }

    Text::Text(const char32_t* text, int size) : Arriba::Graphics::AdvancedTexture(1, 1), Quad(0, 0, 0, 0, Arriba::Graphics::Pivot::centre) {
        renderer->setTexture(texID);
        fontSize = size;
        setText(text);
    }

    void Text::setText(const char* text) {
        char32_t* converted = Arriba::Text::ASCIIToUnicode(text);
        setText(converted);
        free(converted);
    }

    void Text::setText(const char32_t* text) {
        // Vars for storing geometry info
        int xOffset = 0;
        int maxHeight = 0;
        int minHeight = 0;
        // Spawn the chars
        for (unsigned int i = 0; i < std::char_traits<char32_t>::length(text); i++) {
            Arriba::Graphics::CharInfo character = Arriba::Graphics::getChar(text[i], fontSize);
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
        for (unsigned int i = 0; i < std::char_traits<char32_t>::length(text); i++) {
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
