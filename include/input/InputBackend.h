#pragma once

#include <arribaMaths.h>

namespace Arriba::Input {

enum analogStick {
    leftStick,
    rightStick
};

enum controllerButton {
    invalid = 0,
    
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
    XButtonSwitch = northButton,
    AButtonSwitch = eastButton,
    BButtonSwitch = southButton,
    YButtonSwitch = westButton,
    PlusButtonSwitch = optionsRight,
    MinusButtonSwitch = optionsLeft,
    LButtonSwitch = shoulderLeft,
    RButtonSwitch = shoulderRight,
    ZLButtonSwitch = triggerLeft,
    ZRButtonSwitch = triggerRight,

    // Steam / XBOX button names
    YButtonSteam = northButton,
    BButtonSteam = eastButton,
    AButtonSteam = southButton,
    XButtonSteam = westButton,
    L1ButtonSteam = shoulderLeft,
    R1ButtonSteam = shoulderRight,
    L2ButtonSteam = triggerLeft,
    R2ButtonSteam = triggerRight,
};

inline controllerButton confirmButton = invalid;
inline controllerButton backButton = invalid;

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

}  // namespace Arriba::Input
