#pragma once

#include <arriba.h>
#include <arribaMaths.h>
#include <input/InputBackend.h>

namespace Arriba::Input {
struct Touch {
    Arriba::Maths::vec2<float> pos{0};
    Arriba::Maths::vec2<float> delta{0};
    Arriba::Maths::vec2<float> origin{0};
    float downTime = 0;
    bool start = false;
    bool end = false;
};

inline Touch touch;
inline AnalogStick AnalogStickLeft;
inline AnalogStick AnalogStickRight;

void initInput();
void updateHID();
bool buttonHeld(controllerButton button);
bool buttonDown(controllerButton button);
bool buttonUp(controllerButton button);
bool touchScreenPressed();
}  // namespace Arriba::Input
