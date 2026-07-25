#include <arribaInput.h>
#ifdef __SWITCH__
#include <input/InputHOS.h>
#endif

namespace Arriba::Input {
struct ControllerState { int buttons = 0; };
static std::unique_ptr<InputBackend> inputBackend;
static unsigned int kHeld;
static unsigned int kDown;
static unsigned int kUp;
static RawTouch rawTouch;
static bool touchLastFrame = false;
static ControllerState controller;

void initInput() {
    #ifdef __SWITCH__
    inputBackend = std::make_unique<InputHOS>();
    #endif
}

static void applyAxisDPad(int& buttons, float axisPos, bool& axisHeld, bool otherAxisHeld, controllerButton positive, controllerButton negative) {
    if (abs(axisPos) > 0.4f) {
        if (!axisHeld && !otherAxisHeld) buttons |= (axisPos > 0.f) ? positive : negative;
        axisHeld = true;
    } else {
        axisHeld = false;
    }
}

static void controllerUpdate(ControllerState& controller) {
    int buttonsDownLastFrame = controller.buttons;
    inputBackend->updateControllerState();
    controller.buttons = inputBackend->getButtonMask();

    auto leftPos = inputBackend->getStickPos(leftStick);
    AnalogStickLeft.xPos = leftPos.x;
    AnalogStickLeft.yPos = leftPos.y;
    auto rightPos = inputBackend->getStickPos(rightStick);
    AnalogStickRight.xPos = rightPos.x;
    AnalogStickRight.yPos = rightPos.y;

    applyAxisDPad(controller.buttons, AnalogStickLeft.xPos,  AnalogStickLeft.xHeldLastFrame,  AnalogStickLeft.yHeldLastFrame,  DPadRight, DPadLeft);
    applyAxisDPad(controller.buttons, AnalogStickLeft.yPos,  AnalogStickLeft.yHeldLastFrame,  AnalogStickLeft.xHeldLastFrame,  DPadUp,    DPadDown);
    applyAxisDPad(controller.buttons, AnalogStickRight.xPos, AnalogStickRight.xHeldLastFrame, AnalogStickRight.yHeldLastFrame, DPadRight, DPadLeft);
    applyAxisDPad(controller.buttons, AnalogStickRight.yPos, AnalogStickRight.yHeldLastFrame, AnalogStickRight.xHeldLastFrame, DPadUp,    DPadDown);

    kHeld = buttonsDownLastFrame & controller.buttons;
    kUp = buttonsDownLastFrame & ~controller.buttons;
    kDown = controller.buttons & ~buttonsDownLastFrame;
}

static void updateTouchState() {
    rawTouch = inputBackend->getRawTouch();
    if (rawTouch.pressed) {
        if (touchLastFrame) touch.delta = {touch.pos.x - rawTouch.x, touch.pos.y - rawTouch.y};
        touch.pos = {rawTouch.x, rawTouch.y};
        if (!touchLastFrame) touch.origin = touch.pos;
        touch.downTime += Arriba::deltaTime;
        touch.start = !touchLastFrame;
    } else if (!touchLastFrame) {
        touch.downTime = 0;
    }
    touch.end = !rawTouch.pressed && touchLastFrame;
    touchLastFrame = rawTouch.pressed;
}

void updateHID() {
    controllerUpdate(controller);
    updateTouchState();
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
    return rawTouch.pressed;
}
}  // namespace Arriba::Input
