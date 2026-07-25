#ifdef __SWITCH__
#include <InputHOS.h>

namespace Arriba::Input {
InputHOS::InputHOS()
{
    padConfigureInput(1, HidNpadStyleSet_NpadStandard);
    padInitializeDefault(&pad);
    hidInitializeTouchScreen();
    confirmButton = controllerButton::AButtonSwitch;
    backButton = controllerButton::BButtonSwitch;
}

RawTouch InputHOS::getRawTouch() {
    HidTouchScreenState state = {0};
    hidGetTouchScreenStates(&state, 1);
    if (!state.count) return {};
    return {true, static_cast<float>(state.touches[0].x), static_cast<float>(state.touches[0].y)};
}

Arriba::Maths::vec2<float> InputHOS::getStickPos(analogStick stick) {
    int index = (stick == leftStick) ? 0 : 1;
    return {
        padGetStickPos(&pad, index).x / static_cast<float>(JOYSTICK_MAX),
        padGetStickPos(&pad, index).y / static_cast<float>(JOYSTICK_MAX)
    };
}

void InputHOS::updateControllerState() {
    padUpdate(&pad);
    npadKHeld = padGetButtons(&pad);
}

int InputHOS::getButtonMask() {
    static constexpr struct { unsigned int hosKey; int arribaKey; } kMapping[] = {
        {HidNpadButton_X,     XButtonSwitch},
        {HidNpadButton_A,     AButtonSwitch},
        {HidNpadButton_B,     BButtonSwitch},
        {HidNpadButton_Y,     YButtonSwitch},
        {HidNpadButton_Up,    DPadUp},
        {HidNpadButton_Right, DPadRight},
        {HidNpadButton_Down,  DPadDown},
        {HidNpadButton_Left,  DPadLeft},
        {HidNpadButton_Plus,  PlusButtonSwitch},
        {HidNpadButton_Minus, MinusButtonSwitch},
        {HidNpadButton_L,     LButtonSwitch},
        {HidNpadButton_R,     RButtonSwitch},
        {HidNpadButton_ZL,    ZLButtonSwitch},
        {HidNpadButton_ZR,    ZRButtonSwitch},
    };
    int mask = 0;
    for (auto& [hosKey, arribaKey] : kMapping)
        mask |= arribaKey & -(unsigned int)(1 && (npadKHeld & hosKey));
    return mask;
}

}  // namespace Arriba::Input
#endif  // __SWITCH__
