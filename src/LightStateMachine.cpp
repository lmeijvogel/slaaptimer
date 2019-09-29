#pragma once

#include <memory>

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

class LightStateMachine {
  public:
  LightStateMachine(shared_ptr<LightController> lightController) {
    this->lightController = lightController;
    this->lightBlinker = make_unique<LightBlinker>(lightController);
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

    lightController->set(color);
  }

  void buttonPressed(TimeTools::Time &time) {
    switch (this->alarmState) {
      case AlarmState::State::Off:
        toggleThreeState();
        break;
      case AlarmState::State::Prepare:
        toggleTwoState();
        break;
      case AlarmState::State::On:
        toggleTwoState();
        break;
    }

    this->applyState();
  }

  void buttonLongPress(long currentTimeMillis) {
    this->lightBlinker->start(ColorName::Yellow, 2, currentTimeMillis);
  }

  void tick(long currentTimeMillis) {
    this->lightBlinker->tick(currentTimeMillis);
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

  shared_ptr<LightController> lightController;

  AlarmState::State alarmState;

  State state;
  unique_ptr<LightBlinker> lightBlinker;
};
