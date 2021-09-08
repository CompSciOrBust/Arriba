#include <arribaInput.h>

namespace Arriba::Input
{
    void initInput()
    {
        padConfigureInput(1, HidNpadStyleSet_NpadStandard);
        padInitializeDefault(&pad);
        hidInitializeTouchScreen();
    }

    void updateHID()
    {
        padUpdate(&pad);
        kHeld = padGetButtons(&pad);
        kDown = padGetButtonsDown(&pad);
        kUp = padGetButtonsUp(&pad);
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

    bool buttonHeld(HidNpadButton button)
    {
        return (kHeld & button);
    }

    bool buttonDown(HidNpadButton button)
    {
        return (kDown & button);
    }

    bool buttonUp(HidNpadButton button)
    {
        return (kUp & button);
    }

    bool touchScreenPressed()
    {
        return touchState.count;
    }
}