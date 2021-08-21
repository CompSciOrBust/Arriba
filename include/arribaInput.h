#pragma once
#include <arriba.h>
#include <switch.h>
#include <arribaMaths.h>

namespace Arriba::Input
{
    struct Touch
    {
        glm::vec2 pos{0};
        glm::vec2 delta{0};
        glm::vec2 origin{0};
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