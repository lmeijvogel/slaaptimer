#pragma once

#include <memory>

#include "Color.cpp"
#include "LightController.cpp"
#include "lib/TimeTools.cpp"

using namespace Color;
using namespace TimeTools;

enum LightBlinkerState {
  Off,
  BlinkOn,
  BlinkOff
};

class LightBlinker {
  public:
    LightBlinker(std::shared_ptr<LightController> lightController) {
      this->lightController = lightController;
    }

    void start(ColorName color, int times, long startingTimeMillis) {
      this->startingTimeMillis = startingTimeMillis;
      this->blinksLeft = times;
      this->blinkColor = color;

      this->lightController->getState(&this->beforeState);

      this->lightController->turnOff();

      this->nextEventTimeMillis = startingTimeMillis + 250;
      this->state = BlinkOff;
      this->nextState = BlinkOn;
    }

    void tick(long currentTimeMillis) {
      if (this->state == Off) {
        return;
      }

      if (currentTimeMillis < this->nextEventTimeMillis) {
        return;
      }

      switch (this->nextState) {
        case BlinkOn:
            this->lightController->set(this->blinkColor);

            this->state = BlinkOn;
            this->blinksLeft--;

            this->nextState = BlinkOff;
            this->nextEventTimeMillis += 500;
          break;
        case BlinkOff:
          this->lightController->turnOff();

          if (0 < this->blinksLeft) {
            this->nextState = BlinkOn;
          } else {
            this->nextState = Off;
          }

          this->nextEventTimeMillis += 500;

          break;
        case Off:
          this->state = Off;

          this->lightController->setState(this->beforeState);
          this->nextEventTimeMillis = 0;
          break;
      }
    }

  private:
    std::shared_ptr<LightController> lightController;

    LightBlinkerState state = Off;

    ColorName blinkColor = ColorName::None;

    LightControllerState beforeState;
    int blinksLeft = 0;
    long startingTimeMillis = 0;

    long nextEventTimeMillis = 0;
    LightBlinkerState nextState = Off;
};
