#pragma once

#include <font/FontBackend.h>

namespace Arriba::Font {

class FontBackendDesktop : public FontBackend {
public:
    void open(int size) override;
    void close() override;
};

}  // namespace Arriba::Font
