#pragma once

#include <time.h>
#include "AlarmState.cpp"
#include "Color.cpp"
#include "LightBlinker.cpp"
#include "LightController.cpp"
#include "Logger.cpp"

namespace LightState {
  enum State {
    Off,
    Half,
    Full
  };
}

using namespace Color;
using namespace LightState;

const long oneHour = 60l * 60;
const long twelveHours = 12l * oneHour;

class LightStateMachine {
  public:
  LightStateMachine(LightController *lightController, LightBlinker *lightBlinker) {
    this->lightController = lightController;
    this->lightBlinker = lightBlinker;
  }

  void alarmStateChanged(AlarmState::State alarmState) {
    this->alarmState = alarmState;

    switch (alarmState) {
      case AlarmState::State::Off:
        this->state = State::Off;
        break;
      case AlarmState::State::Prepare:
      case AlarmState::State::On:
        this->state = State::Full;
        break;
    }

    this->applyState();
  }

  void applyState() {
    if (this->state == State::Off) {
      lightController->turnOff();
      return;
    }

    if (alarmState == AlarmState::State::Off) {
      float intensity;
      if (this->state == State::Half) {
        intensity = 0.3;
      } else if (this->state == State::Full) {
        intensity = 1;
      } else {
        printf("ILLEGAL STATE!");
      }
      lightController->set(Red, intensity);

      return;
    }

    ColorName color = colorForState(alarmState);

    lightController->set(color, 0.3);
  }

  void toggleLight(long currentTimeMillis) {
    switch (this->alarmState) {
      case AlarmState::State::Off:
        this->lightToggleTime = currentTimeMillis;
        toggleThreeState();
        break;
      case AlarmState::State::Prepare:
      case AlarmState::State::On:
        toggleTwoState();
        break;
    }

    this->applyState();
  }

  // currentTimeMillis is necessary to time the blinks
  void toggleAutoOff(time_t currentTime, long currentTimeMillis) {
    ColorName blinkColor;

    if (this->autoOff) {
      blinkColor = Red;
      Logger::println("Auto-off disabled");
    } else {
      blinkColor = Green;
      Logger::println("Auto-off enabled");
    }

    this->lightBlinker->start(blinkColor, 2, currentTimeMillis);

    this->autoOff = !this->autoOff;

    this->autoOffSettingExpiryTimeMillis = currentTime + twelveHours;
    this->lightToggleTime = currentTime;
  }

  void tick(time_t currentTime) {
    // The autoOff setting is reset after this time
    if (this->autoOffSettingExpiryTimeMillis < currentTime) {
      this->autoOff = false;
    }

    bool canTurnAutoOff = this->state == State::Half || this->state == State::Full;
    if (canTurnAutoOff && this->autoOff) {
      // Only turn off the light if it is Red, not when it's Yellow or Green since they
      // have their own timers
      if (this->alarmState != AlarmState::State::Off) {
        return;
      }

      // TODO: This works, but is badly testable: I can't set a new time
      // in the console and have this respond because it uses the internal
      // Arduino `millis()` function, not our custom time.
      time_t autoOffScheduledTime = this->lightToggleTime + oneHour;

      if (autoOffScheduledTime < currentTime) {
        Logger::println("Auto-off was scheduled. Turning off");

        this->state = State::Off;

        this->applyState();
      }
    }
  }

  private:

  void toggleThreeState() {
    State newState;

    switch (state) {
      case State::Off:
        newState = State::Half;
        break;
      case State::Half:
        newState = State::Full;
        break;
      case State::Full:
        newState = State::Off;
        break;
    }

    this->state = newState;
  }

  void toggleTwoState() {
    if (this->state == State::Off) {
      this->state = State::Full;
    } else {
      this->state = State::Off;
    }
  }

  ColorName colorForState(AlarmState::State alarmState) {
    switch (alarmState) {
      case AlarmState::State::Off:     return Red;
      case AlarmState::State::Prepare: return Yellow;
      case AlarmState::State::On:      return Green;
      default:                         return None;
    }
  }

  LightController *lightController;

  AlarmState::State alarmState;

  State state;
  LightBlinker *lightBlinker;

  time_t lightToggleTime = 0;
  time_t autoOffSettingExpiryTimeMillis = 0;
  bool autoOff = false;
};
