#include <primitives/text.h>
#include <primitives/character.h>
#include <arribaText.h>

namespace Arriba::Primitives {
    Text::Text(const char* text, int size) : Text(Arriba::Text::ASCIIToUnicode(text).c_str(), size) {}

    Text::Text(const char32_t* text, int size) : Quad(0, 0, 0, 0, Arriba::Graphics::Pivot::centre) {
        texture = std::make_unique<Arriba::Graphics::AdvancedTexture>(1,1);
        renderer->setTexture(texture->texID);
        fontSize = size;
        setText(text);
    }

    void Text::setText(const char* text) {
        setText(Arriba::Text::ASCIIToUnicode(text).c_str());
    }

    void Text::setText(const char32_t* text) {
        int xOffset = 0;
        int maxHeight = 0;
        int minHeight = 0;
        std::vector<Arriba::UIObject*> chars;
        for (unsigned int i = 0; i < std::char_traits<char32_t>::length(text); i++) {
            Arriba::Graphics::CharInfo character = Arriba::Graphics::getChar(text[i], fontSize);
            Quad* child = new Character(character);
            chars.push_back(child);
            child->setFBOwner(texture.get());
            child->setColour({1, 1, 1, 1});
            child->transform.position.x = xOffset + character.bearing.x;
            xOffset += (character.advance >> 6);
            maxHeight = (character.size.y > maxHeight) ? character.size.y : maxHeight;
            minHeight = (character.size.y - character.bearing.y > minHeight) ? character.size.y - character.bearing.y : minHeight;
        }
        int yDistance = maxHeight;
        for (unsigned int i = 0; i < std::char_traits<char32_t>::length(text); i++) {
            chars.at(i)->transform.position.y += yDistance;
        }
        setDimensions(xOffset+2, maxHeight + minHeight+2, Arriba::Graphics::Pivot::centre);
        setColour(fontColour);
        texture->resize(width, height);
        updateFrameBuffer(chars);
    }

    void Text::setColour(const Arriba::Maths::vec4<float>& colour) {
        fontColour = colour;
        renderer->setColour(colour);
    }

    void Text::updateFrameBuffer(const std::vector<Arriba::UIObject*>& chars) {
        glBindFramebuffer(GL_FRAMEBUFFER, texture->FBO);
        glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
        glClear(GL_COLOR_BUFFER_BIT);
        for (auto* c : chars) drawTextureObject(c);
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        for (auto* c : chars) c->destroy();
    }
}  // namespace Arriba::Primitives
