#include <arribaInput.h>

namespace Arriba::Input
{
    void initInput()
    {
        //HOS init code
        #ifdef __SWITCH__
        padConfigureInput(1, HidNpadStyleSet_NpadStandard);
        padInitializeDefault(&pad);
        hidInitializeTouchScreen();
        #endif
    }

    void controllerUpdate(ControllerState* _controller)
    {
        //Get pad input for HOS
        #ifdef __SWITCH__
        int buttonsDownLastFrame = _controller->buttons;
        _controller->buttons = 0;
        padUpdate(&pad);
        int npadKHeld = padGetButtons(&pad);
        //Set button pressed bits
        //First AND the npad button bit with the pressed bits
        //Then AND that with 1 to give 1 when the button is pressed and 0 otherwise
        //Use some bit hacking to turn the 1 in to 0xFFFF....
        //Xor that with the Arriba button bit
        //Finally OR it with the pressed bits and update the pressed bits in the struct

        //ABXY
        _controller->buttons = _controller->buttons | (controllerButton::XButtonSwitch & -(unsigned int)(1 && (npadKHeld & HidNpadButton_X)));
        _controller->buttons = _controller->buttons | (controllerButton::AButtonSwitch & -(unsigned int)(1 && (npadKHeld & HidNpadButton_A)));
        _controller->buttons = _controller->buttons | (controllerButton::BButtonSwitch & -(unsigned int)(1 && (npadKHeld & HidNpadButton_B)));
        _controller->buttons = _controller->buttons | (controllerButton::YButtonSwitch & -(unsigned int)(1 && (npadKHeld & HidNpadButton_Y)));

        //DPAD
        _controller->buttons = _controller->buttons | (controllerButton::DPadUp & -(unsigned int)(1 && (npadKHeld & HidNpadButton_Up)));
        _controller->buttons = _controller->buttons | (controllerButton::DPadRight & -(unsigned int)(1 && (npadKHeld & HidNpadButton_Right)));
        _controller->buttons = _controller->buttons | (controllerButton::DPadDown & -(unsigned int)(1 && (npadKHeld & HidNpadButton_Down)));
        _controller->buttons = _controller->buttons | (controllerButton::DPadLeft & -(unsigned int)(1 && (npadKHeld & HidNpadButton_Left)));

        //PLUS / MINUS
        _controller->buttons = _controller->buttons | (controllerButton::PlusButtonSwitch & -(unsigned int)(1 && (npadKHeld & HidNpadButton_Plus)));
        _controller->buttons = _controller->buttons | (controllerButton::MinusButtonSwitch & -(unsigned int)(1 && (npadKHeld & HidNpadButton_Minus)));

        //Triggers / Shoulder buttons
        _controller->buttons = _controller->buttons | (controllerButton::LButtonSwitch & -(unsigned int)(1 && (npadKHeld & HidNpadButton_L)));
        _controller->buttons = _controller->buttons | (controllerButton::RButtonSwitch & -(unsigned int)(1 && (npadKHeld & HidNpadButton_R)));
        _controller->buttons = _controller->buttons | (controllerButton::ZLButtonSwitch & -(unsigned int)(1 && (npadKHeld & HidNpadButton_ZL)));
        _controller->buttons = _controller->buttons | (controllerButton::ZRButtonSwitch & -(unsigned int)(1 && (npadKHeld & HidNpadButton_ZR)));
        
        kHeld = buttonsDownLastFrame & _controller->buttons;
        kUp = buttonsDownLastFrame & ~_controller->buttons;
        kDown = _controller->buttons & ~buttonsDownLastFrame;
        #endif
    }

    void updateHID()
    {
        controllerUpdate(&controller);

        hidGetTouchScreenStates(&touchState, 1);
        if (touchScreenPressed())
        {
            if(touchLastFrame) touch.delta = {touch.pos.x - touchState.touches[0].x, touch.pos.y - touchState.touches[0].y};
            touch.pos = {touchState.touches[0].x, touchState.touches[0].y};
            if (!touchLastFrame) touch.origin = touch.pos;
            touch.downTime += Arriba::deltaTime;
            touch.start = !touchLastFrame;
        }
        else
        {
            touch.downTime = 0;
        }
        touch.end = !touchScreenPressed() && touchLastFrame;
        touchLastFrame = touchScreenPressed();
    }

    bool buttonHeld(controllerButton button)
    {
        return (kHeld & button);
    }

    bool buttonDown(controllerButton button)
    {
        return (kDown & button);
    }

    bool buttonUp(controllerButton button)
    {
        return (kUp & button);
    }

    bool touchScreenPressed()
    {
        return touchState.count;
    }
}