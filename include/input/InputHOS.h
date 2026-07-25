#pragma once

#include <InputBackend.h>
#include <switch.h>

namespace Arriba::Input {
class InputHOS : public InputBackend {
    private:
    PadState pad;
    int npadKHeld = 0;

    public:
    InputHOS();
    virtual void updateControllerState();
    virtual int getButtonMask();
    virtual Arriba::Maths::vec2<float> getStickPos(analogStick stick);
    virtual RawTouch getRawTouch();
};
} // namespace Arriba::Input
