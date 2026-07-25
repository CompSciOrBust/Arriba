#ifdef __linux__
#include <InputSDL.h>
#include <arribaGraphics.h>
#include <GLFW/glfw3.h>

namespace Arriba::Input {

InputSDL::InputSDL() {
    SDL_InitSubSystem(SDL_INIT_GAMEPAD | SDL_INIT_EVENTS);
    int count;
    SDL_JoystickID* gamepads = SDL_GetGamepads(&count);
    if (count > 0) gamepad = SDL_OpenGamepad(gamepads[0]);
    SDL_free(gamepads);
    confirmButton = controllerButton::AButtonSteam;
    backButton = controllerButton::BButtonSteam;
}

InputSDL::~InputSDL() {
    if (gamepad) SDL_CloseGamepad(gamepad);
    SDL_QuitSubSystem(SDL_INIT_GAMEPAD | SDL_INIT_EVENTS);
}

void InputSDL::updateControllerState() {
    SDL_Event event;
    while (SDL_PollEvent(&event)) {
        switch (event.type) {
        case SDL_EVENT_FINGER_DOWN:
        case SDL_EVENT_FINGER_MOTION:
            currentTouch = {true,
                event.tfinger.x * Arriba::Graphics::windowWidth,
                event.tfinger.y * Arriba::Graphics::windowHeight};
            break;
        case SDL_EVENT_FINGER_UP:
            currentTouch = {};
            break;
        case SDL_EVENT_GAMEPAD_ADDED:
            if (!gamepad) gamepad = SDL_OpenGamepad(event.gdevice.which);
            break;
        case SDL_EVENT_GAMEPAD_REMOVED:
            if (gamepad && SDL_GetGamepadID(gamepad) == event.gdevice.which) {
                SDL_CloseGamepad(gamepad);
                gamepad = nullptr;
            }
            break;
        }
    }
}

int InputSDL::getButtonMask() {
    if (!gamepad) return 0;
    static constexpr struct { SDL_GamepadButton sdlButton; int arribaButton; } kMapping[] = {
        {SDL_GAMEPAD_BUTTON_NORTH,          northButton},
        {SDL_GAMEPAD_BUTTON_EAST,           eastButton},
        {SDL_GAMEPAD_BUTTON_SOUTH,          southButton},
        {SDL_GAMEPAD_BUTTON_WEST,           westButton},
        {SDL_GAMEPAD_BUTTON_DPAD_UP,        DPadUp},
        {SDL_GAMEPAD_BUTTON_DPAD_RIGHT,     DPadRight},
        {SDL_GAMEPAD_BUTTON_DPAD_DOWN,      DPadDown},
        {SDL_GAMEPAD_BUTTON_DPAD_LEFT,      DPadLeft},
        {SDL_GAMEPAD_BUTTON_START,          optionsRight},
        {SDL_GAMEPAD_BUTTON_BACK,           optionsLeft},
        {SDL_GAMEPAD_BUTTON_LEFT_SHOULDER,  shoulderLeft},
        {SDL_GAMEPAD_BUTTON_RIGHT_SHOULDER, shoulderRight},
    };
    int mask = 0;
    for (auto& [sdlButton, arribaButton] : kMapping)
        mask |= arribaButton & -(int)SDL_GetGamepadButton(gamepad, sdlButton);
    if (SDL_GetGamepadAxis(gamepad, SDL_GAMEPAD_AXIS_LEFT_TRIGGER) > 16384) mask |= triggerLeft;
    if (SDL_GetGamepadAxis(gamepad, SDL_GAMEPAD_AXIS_RIGHT_TRIGGER) > 16384) mask |= triggerRight;
    return mask;
}

Arriba::Maths::vec2<float> InputSDL::getStickPos(analogStick stick) {
    if (!gamepad) return {0, 0};
    SDL_GamepadAxis axisX = (stick == leftStick) ? SDL_GAMEPAD_AXIS_LEFTX : SDL_GAMEPAD_AXIS_RIGHTX;
    SDL_GamepadAxis axisY = (stick == leftStick) ? SDL_GAMEPAD_AXIS_LEFTY : SDL_GAMEPAD_AXIS_RIGHTY;
    float xValue = SDL_GetGamepadAxis(gamepad, axisX) / 32767.0f;
    if (std::abs(xValue) < 0.12) xValue = 0.;
    float yValue = -SDL_GetGamepadAxis(gamepad, axisY) / 32767.0f;
    if (std::abs(yValue) < 0.12) yValue = 0.;
    return {
        xValue,
        yValue
    };
}

RawTouch InputSDL::getRawTouch() {
    if (!Arriba::Graphics::window) return {};
    if (glfwGetMouseButton(Arriba::Graphics::window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS) {
        double x, y;
        glfwGetCursorPos(Arriba::Graphics::window, &x, &y);
        return {true, static_cast<float>(x), static_cast<float>(y)};
    }
    return {};
}

}  // namespace Arriba::Input
#endif  // __linux__
