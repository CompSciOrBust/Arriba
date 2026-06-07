#include <arribaInput.h>

namespace Arriba::Input {
    void initInput() {
        // HOS init code
        #ifdef __SWITCH__
        padConfigureInput(1, HidNpadStyleSet_NpadStandard);
        padInitializeDefault(&pad);
        hidInitializeTouchScreen();
        #endif
    }

    void controllerUpdate(ControllerState* controller) {
        // Get pad input for HOS
        #ifdef __SWITCH__
        int buttonsDownLastFrame = controller->buttons;
        controller->buttons = 0;
        padUpdate(&pad);
        int npadKHeld = padGetButtons(&pad);
        // Set button pressed bits
        // First AND the npad button bit with the pressed bits
        // Then AND that with 1 to give 1 when the button is pressed and 0 otherwise
        // Use some bit hacking to turn the 1 in to 0xFFFF....
        // Xor that with the Arriba button bit
        // Finally OR it with the pressed bits and update the pressed bits in the struct

        // ABXY
        controller->buttons = controller->buttons | (controllerButton::XButtonSwitch & -(unsigned int)(1 && (npadKHeld & HidNpadButton_X)));
        controller->buttons = controller->buttons | (controllerButton::AButtonSwitch & -(unsigned int)(1 && (npadKHeld & HidNpadButton_A)));
        controller->buttons = controller->buttons | (controllerButton::BButtonSwitch & -(unsigned int)(1 && (npadKHeld & HidNpadButton_B)));
        controller->buttons = controller->buttons | (controllerButton::YButtonSwitch & -(unsigned int)(1 && (npadKHeld & HidNpadButton_Y)));

        // DPAD
        controller->buttons = controller->buttons | (controllerButton::DPadUp & -(unsigned int)(1 && (npadKHeld & HidNpadButton_Up)));
        controller->buttons = controller->buttons | (controllerButton::DPadRight & -(unsigned int)(1 && (npadKHeld & HidNpadButton_Right)));
        controller->buttons = controller->buttons | (controllerButton::DPadDown & -(unsigned int)(1 && (npadKHeld & HidNpadButton_Down)));
        controller->buttons = controller->buttons | (controllerButton::DPadLeft & -(unsigned int)(1 && (npadKHeld & HidNpadButton_Left)));

        // PLUS / MINUS
        controller->buttons = controller->buttons | (controllerButton::PlusButtonSwitch & -(unsigned int)(1 && (npadKHeld & HidNpadButton_Plus)));
        controller->buttons = controller->buttons | (controllerButton::MinusButtonSwitch & -(unsigned int)(1 && (npadKHeld & HidNpadButton_Minus)));

        // Triggers / Shoulder buttons
        controller->buttons = controller->buttons | (controllerButton::LButtonSwitch & -(unsigned int)(1 && (npadKHeld & HidNpadButton_L)));
        controller->buttons = controller->buttons | (controllerButton::RButtonSwitch & -(unsigned int)(1 && (npadKHeld & HidNpadButton_R)));
        controller->buttons = controller->buttons | (controllerButton::ZLButtonSwitch & -(unsigned int)(1 && (npadKHeld & HidNpadButton_ZL)));
        controller->buttons = controller->buttons | (controllerButton::ZRButtonSwitch & -(unsigned int)(1 && (npadKHeld & HidNpadButton_ZR)));

        // Update analog stick values
        AnalogStickLeft.xPos = padGetStickPos(&pad, 0).x / static_cast<float>(JOYSTICK_MAX);
        AnalogStickLeft.yPos = padGetStickPos(&pad, 0).y / static_cast<float>(JOYSTICK_MAX);
        AnalogStickRight.xPos = padGetStickPos(&pad, 1).x / static_cast<float>(JOYSTICK_MAX);
        AnalogStickRight.yPos = padGetStickPos(&pad, 1).y / static_cast<float>(JOYSTICK_MAX);

        // Treat taps as DPAD input
        // Left stick x
        if (abs(AnalogStickLeft.xPos) > 0.4) {
            if (!AnalogStickLeft.xHeldLastFrame && !AnalogStickLeft.yHeldLastFrame) {
                if (AnalogStickLeft.xPos > 0.) {
                    controller->buttons = controller->buttons | controllerButton::DPadRight;
                } else {
                    controller->buttons = controller->buttons | controllerButton::DPadLeft;
                }
            }
            AnalogStickLeft.xHeldLastFrame = true;
        } else {
            AnalogStickLeft.xHeldLastFrame = false;
        }

        // Left stick y
        if (abs(AnalogStickLeft.yPos) > 0.4) {
            if (!AnalogStickLeft.yHeldLastFrame && !AnalogStickLeft.xHeldLastFrame) {
                if (AnalogStickLeft.yPos > 0.) {
                    controller->buttons = controller->buttons | controllerButton::DPadUp;
                } else {
                    controller->buttons = controller->buttons | controllerButton::DPadDown;
                }
            }
            AnalogStickLeft.yHeldLastFrame = true;
        } else {
            AnalogStickLeft.yHeldLastFrame = false;
        }

        // Right stick x
        if (abs(AnalogStickRight.xPos) > 0.4) {
            if (!AnalogStickRight.xHeldLastFrame && !AnalogStickRight.yHeldLastFrame) {
                if (AnalogStickRight.xPos > 0.) {
                    controller->buttons = controller->buttons | controllerButton::DPadRight;
                } else {
                    controller->buttons = controller->buttons | controllerButton::DPadLeft;
                }
            }
            AnalogStickRight.xHeldLastFrame = true;
        } else {
            AnalogStickRight.xHeldLastFrame = false;
        }

        // Right stick y
        if (abs(AnalogStickRight.yPos) > 0.4) {
            if (!AnalogStickRight.yHeldLastFrame && !AnalogStickRight.xHeldLastFrame) {
                if (AnalogStickRight.yPos > 0.) {
                    controller->buttons = controller->buttons | controllerButton::DPadUp;
                } else {
                    controller->buttons = controller->buttons | controllerButton::DPadDown;
                }
            }
            AnalogStickRight.yHeldLastFrame = true;
        } else {
            AnalogStickRight.yHeldLastFrame = false;
        }

        kHeld = buttonsDownLastFrame & controller->buttons;
        kUp = buttonsDownLastFrame & ~controller->buttons;
        kDown = controller->buttons & ~buttonsDownLastFrame;
        #endif
    }

    void updateHID() {
        controllerUpdate(&controller);

        hidGetTouchScreenStates(&touchState, 1);
        if (touchScreenPressed()) {
            if (touchLastFrame) touch.delta = {touch.pos.x - touchState.touches[0].x, touch.pos.y - touchState.touches[0].y};
            touch.pos = {touchState.touches[0].x, touchState.touches[0].y};
            if (!touchLastFrame) touch.origin = touch.pos;
            touch.downTime += Arriba::deltaTime;
            touch.start = !touchLastFrame;
        } else if (!touchLastFrame) {
            touch.downTime = 0;
        }
        touch.end = !touchScreenPressed() && touchLastFrame;
        touchLastFrame = touchScreenPressed();
    }

    bool buttonHeld(controllerButton button) {
        return (kHeld & button);
    }

    bool buttonDown(controllerButton button) {
        return (kDown & button);
    }

    bool buttonUp(controllerButton button) {
        return (kUp & button);
    }

    bool touchScreenPressed() {
        return touchState.count;
    }
}  // namespace Arriba::Input
