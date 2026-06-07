#pragma once

#include <vector>
#include <string>

namespace Arriba::Text {
    std::u32string ASCIIToUnicode(const char* text);
    std::vector<std::u32string> ASCIIToUnicodeList(const std::vector<std::string>& strings);
} // namespace Arriba::Text
