#include <arribaInput.h>


void Arriba::Input::initInput()
{
    padConfigureInput(1, HidNpadStyleSet_NpadStandard);
    padInitializeDefault(&Arriba::Input::pad);
    hidInitializeTouchScreen();
}

void Arriba::Input::updateHID()
{
    padUpdate(&pad);
    kHeld = padGetButtons(&pad);
    kDown = padGetButtonsDown(&pad);
    kUp = padGetButtonsUp(&pad);
    hidGetTouchScreenStates(&touchState, 1);
    if (touchScreenPressed())
    {
        touch.delta = {touch.pos.x - touchState.touches[0].x, touch.pos.y - touchState.touches[0].y};
        touch.pos = {touchState.touches[0].x, touchState.touches[0].y};
        if (!touchLastFrame) touch.origin = touch.pos;
        touch.downTime += Arriba::deltaTime;
        touch.start = !touchLastFrame;
    }
    else touch.downTime = 0;
    touchLastFrame = touchScreenPressed();
}

bool Arriba::Input::buttonHeld(HidNpadButton button)
{
    return (kHeld & button);
}

bool Arriba::Input::buttonDown(HidNpadButton button)
{
    return (kDown & button);
}

bool Arriba::Input::buttonUp(HidNpadButton button)
{
    return (kUp & button);
}

bool Arriba::Input::touchScreenPressed()
{
    return touchState.count;
}