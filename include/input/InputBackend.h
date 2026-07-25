#pragma once

#include <arribaMaths.h>

namespace Arriba::Input {

enum analogStick {
    leftStick,
    rightStick
};
    enum controllerButton {
    // Generic buttons for all platforms
    northButton = 1,
    eastButton = 2,
    southButton = 4,
    westButton = 8,
    DPadUp = 16,
    DPadRight = 32,
    DPadDown = 64,
    DPadLeft = 128,
    optionsRight = 512,
    optionsLeft = 1024,
    shoulderLeft = 2048,
    shoulderRight = 4096,
    triggerLeft = 8192,
    triggerRight = 16384,

    // Switch specific button names
    XButtonSwitch = 1,
    AButtonSwitch = 2,
    BButtonSwitch = 4,
    YButtonSwitch = 8,
    PlusButtonSwitch = 512,
    MinusButtonSwitch = 1024,
    LButtonSwitch = 2048,
    RButtonSwitch = 4096,
    ZLButtonSwitch = 8192,
    ZRButtonSwitch = 16384
};

struct AnalogStick {
    float xPos = 0;
    float yPos = 0;
    bool xHeldLastFrame = false;
    bool yHeldLastFrame = false;
};

struct RawTouch {
    bool pressed = false;
    float x = 0;
    float y = 0;
};

class InputBackend {
    public:
    virtual ~InputBackend() {};
    virtual void updateControllerState() = 0;
    virtual int getButtonMask() = 0;
    virtual Arriba::Maths::vec2<float> getStickPos(analogStick stick) = 0;
    virtual RawTouch getRawTouch() = 0;
};
} // namespace Arriba::Input
