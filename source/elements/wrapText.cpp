#include <elements/wrapText.h>
#include <arribaText.h>
#include <vector>

namespace Arriba::Elements {

using Arriba::Primitives::Text;

WrapText::WrapText(const char* text, int size, int wrapWidth)
    : WrapText(Arriba::Text::ASCIIToUnicode(text).c_str(), size, wrapWidth) {}

WrapText::WrapText(const char32_t* text, int size, int wrapWidth)
    : Text(U"", size), wrapWidth(wrapWidth) {
    setText(text);
}

void WrapText::setText(const char* text) {
    setText(Arriba::Text::ASCIIToUnicode(text).c_str());
}

void WrapText::setText(const char32_t* text) {
    storedText = text;
    std::u32string fullText = text;
    size_t textLen = fullText.length();

    if (textLen == 0) {
        setDimensions(0, 0, Arriba::Graphics::Pivot::centre);
        texture->resize(this->width, this->height);
        std::vector<Arriba::UIObject*> emptyObjs;
        updateFrameBuffer(emptyObjs);
        return;
    }

    std::vector<int> prefixWidths(textLen + 1, 0);
    for (size_t i = 0; i < textLen; i++) {
        Arriba::Graphics::CharInfo ch = Arriba::Graphics::getChar(fullText[i], fontSize);
        prefixWidths[i + 1] = prefixWidths[i] + (ch.advance >> 6);
    }

    std::vector<size_t> spaceIndices;
    for (size_t i = 0; i < textLen; i++) {
        if (fullText[i] == U' ') spaceIndices.push_back(i);
    }
    spaceIndices.push_back(textLen);

    int yOffset = 0;
    int totalMaxW = 0;
    std::vector<Arriba::UIObject*> objs;

    struct LineInfo {
        Arriba::UIObject* obj;
        int width;
        int height;
    };
    std::vector<LineInfo> lines;

    size_t lineStart = 0;

    while (lineStart < textLen) {
        size_t lowIdx = 0;
        while (lowIdx < spaceIndices.size() && spaceIndices[lowIdx] <= lineStart) {
            lowIdx++;
        }

        if (lowIdx >= spaceIndices.size()) break;

        size_t low = lowIdx;
        size_t high = spaceIndices.size() - 1;

        size_t bestEnd = spaceIndices[lowIdx];
        int bestWidth = -1;

        while (low <= high) {
            size_t mid = low + (high - low) / 2;
            size_t testEnd = spaceIndices[mid];

            int width = prefixWidths[testEnd] - prefixWidths[lineStart] + 2;
            if (width <= wrapWidth) {
                bestEnd = testEnd;
                bestWidth = width;
                low = mid + 1;
            } else {
                if (mid == 0) break;
                high = mid - 1;
            }
        }

        if (bestWidth == -1) bestWidth = prefixWidths[bestEnd] - prefixWidths[lineStart] + 2;

        std::u32string lineStr = fullText.substr(lineStart, bestEnd - lineStart);
        Text* lineObj = new Text(lineStr.c_str(), fontSize);
        lineObj->setDimensions(lineObj->width, lineObj->height, Arriba::Graphics::topLeft);

        lineObj->transform.position.x = 0;
        lineObj->transform.position.y = yOffset;
        lineObj->setColour({1.0f, 1.0f, 1.0f, 1.0f});
        lineObj->setFBOwner(texture.get());

        objs.push_back(lineObj);
        lines.push_back({lineObj, lineObj->width, lineObj->height});

        totalMaxW = std::max(totalMaxW, lineObj->width);
        yOffset += lineObj->height;

        lineStart = bestEnd + 1;
    }

    int totalMaxH = yOffset;

    if (centreJustified) {
        for (const LineInfo& line : lines) {
            line.obj->transform.position.x = (totalMaxW - line.width) / 2;
        }
    }

    setDimensions(totalMaxW, totalMaxH, Arriba::Graphics::Pivot::centre);
    setColour(fontColour);
    texture->resize(this->width, this->height);
    updateFrameBuffer(objs);
}

void WrapText::setWrapWidth(int newWidth) {
    wrapWidth = newWidth;
    std::u32string copy = storedText;
    setText(copy.c_str());
}

}  // namespace Arriba::Elements
