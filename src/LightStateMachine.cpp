#pragma once

#include "AlarmState.cpp"
#include "Color.cpp"
#include "LightBlinker.cpp"
#include "LightController.cpp"
#include "lib/TimeTools.cpp"

namespace LightState {
  enum State {
    Off,
    Half,
    Full
  };
}

using namespace std;
using namespace Color;
using namespace LightState;

const long oneHour = 60 * 60 * 1000;
const long twelveHours = 12 * oneHour;

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
        intensity = 0.5;
      } else if (this->state == State::Full) {
        intensity = 1;
      } else {
        printf("ILLEGAL STATE!");
      }
      lightController->set(Red, intensity);

      return;
    }

    ColorName color = colorForState(alarmState);

    lightController->set(color, 1.0);
  }

  void toggleLight(long currentTimeMillis) {
    switch (this->alarmState) {
      case AlarmState::State::Off:
        this->lightToggleTimeMillis = currentTimeMillis;
        toggleThreeState();
        break;
      case AlarmState::State::Prepare:
      case AlarmState::State::On:
        toggleTwoState();
        break;
    }

    this->applyState();
  }

  // The current time is necessary to time the blinks
  void toggleAutoOff(long currentTimeMillis) {
    ColorName blinkColor;

    if (this->autoOff) {
      blinkColor = Red;
    } else {
      blinkColor = Green;
    }

    this->lightBlinker->start(blinkColor, 2, currentTimeMillis);

    this->autoOff = !this->autoOff;

    this->autoOffSettingExpiryTimeMillis = currentTimeMillis + twelveHours;
    this->lightToggleTimeMillis = currentTimeMillis;
  }

  void tick(long currentTimeMillis) {
    // The autoOff setting is reset after this time
    if (this->autoOffSettingExpiryTimeMillis < currentTimeMillis) {
      this->autoOff = false;
    }

    long autoOffSettingRetentionTimeMillis = 12 * 60 * 60 * 1000;

    this->lightBlinker->tick(currentTimeMillis);

    // Only turn off the light if it is Red, not when it's Yellow or Green since they
    // have their own timers
    if (this->alarmState == AlarmState::State::Off) {
      if (this->autoOff && (this->lightToggleTimeMillis + oneHour < currentTimeMillis)) {
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
    }
  }

  LightController *lightController;

  AlarmState::State alarmState;

  State state;
  LightBlinker *lightBlinker;

  long lightToggleTimeMillis = 0;
  long autoOffSettingExpiryTimeMillis = 0;
  bool autoOff = false;
};
