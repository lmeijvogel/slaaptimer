#pragma once

#include "AlarmState.cpp"
#include "lib/TimeTools.cpp"

namespace LightState {
  enum State {
    Off,
    Half,
    Full
  };
}

class LightStateMachine {
  public:
  LightStateMachine(shared_ptr<LightController> lightController) {
    this->lightController = lightController;
  }

  void alarmStateChanged(AlarmState::State alarmState) {
    this->alarmState = alarmState;

    switch (alarmState) {
      case AlarmState::State::Off:
        this->state = LightState::State::Off;
        break;
      case AlarmState::State::Prepare:
      case AlarmState::State::On:
        this->state = LightState::State::Full;
        break;
    }

    this->applyState();
  }

  void applyState() {
    if (this->state == LightState::State::Off) {
      lightController->turnOff();
      return;
    }

    switch (alarmState) {
      case AlarmState::State::Off:
        if (this->state == LightState::State::Half) {
          lightController->setRedHalf();
        } else if (this->state == LightState::State::Full) {
          lightController->setRedFull();
        } else {
          printf("ILLEGAL STATE!");
        }
        break;
      case AlarmState::State::Prepare:
        lightController->setYellow();
        break;
      case AlarmState::State::On:
        lightController->setGreen();
        break;
    }
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

  void tick(TimeTools::Time &time) {
    this->currentTime.hour = time.hour;
    this->currentTime.minute = time.minute;
  }

  private:

  void toggleThreeState() {
    LightState::State newState;

    switch (state) {
      case LightState::State::Off:
        newState = LightState::State::Half;
        break;
      case LightState::State::Half:
        newState = LightState::State::Full;
        break;
      case LightState::State::Full:
        newState = LightState::State::Off;
        break;
    }

    this->state = newState;
  }

  void toggleTwoState() {
    if (this->state == LightState::State::Off) {
      this->state = LightState::State::Full;
    } else {
      this->state = LightState::State::Off;
    }
  }

  shared_ptr<LightController> lightController;

  // TODO: This will go away
  TimeTools::Time currentTime;
  AlarmState::State alarmState;

  LightState::State state;
};
