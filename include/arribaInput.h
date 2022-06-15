#pragma once
#include <arriba.h>
#include <switch.h>
#include <arribaMaths.h>

namespace Arriba::Input
{
    struct Touch
    {
        Arriba::Maths::vec2<float> pos{0};
        Arriba::Maths::vec2<float> delta{0};
        Arriba::Maths::vec2<float> origin{0};
        float downTime = 0;
        bool start;
        bool end;
    };

    inline PadState pad;
    inline HidTouchScreenState touchState = {0};
    inline unsigned int kHeld;
    inline unsigned int kDown;
    inline unsigned int kUp;
    inline bool touchLastFrame = false;
    inline Touch touch;

    void initInput();
    void updateHID();
    bool buttonHeld(HidNpadButton);
    bool buttonDown(HidNpadButton);
    bool buttonUp(HidNpadButton);
    bool touchScreenPressed();
}