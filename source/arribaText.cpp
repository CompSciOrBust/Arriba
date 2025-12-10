#include <arribaText.h>
#include <stdio.h>
#include <cstring>
#include <cstdlib>
#include <vector>
#include <string>

namespace Arriba::Text {
    char32_t* ASCIIToUnicode(const char* text) {
        int textLength = strlen(text);
        char32_t* converted = (char32_t*)calloc(textLength + 1, sizeof(char32_t));
        for (int i = 0; i < textLength; i++) converted[i] = (char32_t)text[i];
        return converted;
    }

    std::vector<std::u32string> ASCIIToUnicodeList(std::vector<std::string> strings) {
        std::vector<std::u32string> _strings = {};
        for (std::string str: strings) {
            char32_t* converted = Arriba::Text::ASCIIToUnicode(str.c_str());
            _strings.push_back(converted);
            free(converted);
        }
        return _strings;
    }
} // namepace Arriba::Text
