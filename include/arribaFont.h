#pragma once

#include <memory>
#include <font/FontBackend.h>
#ifdef __SWITCH__
#include <font/FontBackendHOS.h>
#else
#include <font/FontBackendDesktop.h>
#include <string>
#endif

namespace Arriba::Font {

#ifndef __SWITCH__
inline std::string fontPath;
#endif

inline std::unique_ptr<FontBackend> fontBackend;

inline void initFontBackend() {
    #ifdef __SWITCH__
    fontBackend = std::make_unique<FontBackendHOS>();
    #else
    fontBackend = std::make_unique<FontBackendDesktop>();
    #endif
}

}  // namespace Arriba::Font
