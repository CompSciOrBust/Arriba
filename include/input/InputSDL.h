#pragma once

#include <InputBackend.h>
#include <SDL3/SDL.h>

namespace Arriba::Input {
class InputSDL : public InputBackend {
private:
    SDL_Gamepad* gamepad = nullptr;
    RawTouch currentTouch;

public:
    InputSDL();
    ~InputSDL();
    virtual void updateControllerState();
    virtual int getButtonMask();
    virtual Arriba::Maths::vec2<float> getStickPos(analogStick stick);
    virtual RawTouch getRawTouch();
};
}  // namespace Arriba::Input
