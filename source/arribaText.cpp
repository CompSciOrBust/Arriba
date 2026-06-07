#include <arribaText.h>
#include <vector>
#include <string>

namespace Arriba::Text {
    std::u32string ASCIIToUnicode(const char* text) {
        std::u32string result;
        while (*text) result += static_cast<char32_t>(*text++);
        return result;
    }

    std::vector<std::u32string> ASCIIToUnicodeList(const std::vector<std::string>& strings) {
        std::vector<std::u32string> _strings = {};
        for (const std::string& str : strings) {
            _strings.push_back(Arriba::Text::ASCIIToUnicode(str.c_str()));
        }
        return _strings;
    }
} // namespace Arriba::Text
