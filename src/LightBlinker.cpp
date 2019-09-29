#pragma once

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
    LightBlinker(LightController *lightController) {
      this->lightController = lightController;
    }

    void start(ColorName color, int times, long startingTimeMillis) {
      if (this->isEnabled) {
        return; // Don't start blinking twice to prevent loss of previous state
      }

      this->startingTimeMillis = startingTimeMillis;
      this->blinksLeft = times;
      this->blinkColor = color;

      this->lightController->getState(&this->beforeState);

      this->lightController->turnOff();

      this->nextEventTimeMillis = startingTimeMillis + 250;

      this->isEnabled = true;

      this->nextState = BlinkOn;
    }

    void tick(long currentTimeMillis) {
      if (!this->isEnabled) {
        return;
      }

      if (currentTimeMillis < this->nextEventTimeMillis) {
        return;
      }

      switch (this->nextState) {
        case BlinkOn:
            this->lightController->set(this->blinkColor, 1.0);

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
          this->lightController->setState(this->beforeState);
          this->nextEventTimeMillis = 0;
          this->isEnabled = false;
          break;
      }
    }

  private:
    LightController *lightController;

    bool isEnabled = false;

    ColorName blinkColor = ColorName::None;

    LightControllerState beforeState;
    int blinksLeft = 0;
    long startingTimeMillis = 0;

    long nextEventTimeMillis = 0;
    LightBlinkerState nextState = Off;
};
