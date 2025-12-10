#pragma once
#include <vector>
#include <string>

namespace Arriba::Text {
    char32_t* ASCIIToUnicode(const char* text);
    std::vector<std::u32string> ASCIIToUnicodeList(std::vector<std::string> strings);
} // namepace Arriba::Text
