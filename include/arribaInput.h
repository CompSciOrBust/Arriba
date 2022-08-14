#pragma once
#include <arriba.h>
#include <switch.h>
#include <arribaMaths.h>

namespace Arriba::Input
{

    enum controllerButton {
        //Generic buttons for all platforms
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
        shoulderLeft = 2028,
        shoulderRight = 4096,
        triggerLeft = 8192,
        triggerRight = 16384,
        //Switch specific button names
        XButtonSwitch = 1,
        AButtonSwitch = 2,
        BButtonSwitch = 4,
        YButtonSwitch = 8,
        PlusButtonSwitch = 512,
        MinusButtonSwitch = 1024,
        LButtonSwitch = 2028,
        RButtonSwitch = 4096,
        ZLButtonSwitch = 8192,
        ZRButtonSwitch = 16384
    };

    struct Touch
    {
        Arriba::Maths::vec2<float> pos{0};
        Arriba::Maths::vec2<float> delta{0};
        Arriba::Maths::vec2<float> origin{0};
        float downTime = 0;
        bool start;
        bool end;
    };

    struct ControllerState
    {
        int buttons = 0;
        int buttonsDown = 0;
        int buttonsUp = 0;
    };

    //Variables needed for HOS
    #ifdef __SWITCH__
    inline PadState pad;
    #endif
    inline HidTouchScreenState touchState = {0};
    //Generic variables
    inline unsigned int kHeld;
    inline unsigned int kDown;
    inline unsigned int kUp;
    inline bool touchLastFrame = false;
    inline Touch touch;
    inline ControllerState controller;

    void initInput();
    void controllerUpdate(ControllerState*);
    void updateHID();
    bool buttonHeld(controllerButton);
    bool buttonDown(controllerButton);
    bool buttonUp(controllerButton);
    bool touchScreenPressed();
}