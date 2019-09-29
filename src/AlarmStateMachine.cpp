#pragma once

#include "LightStateMachine.cpp"
#include "AlarmState.cpp"
#include "lib/TimeTools.cpp"

const TimeTools::Time PREPARE_TIME = { 6, 45 };
const TimeTools::Time ON_TIME = { 7, 15 };
const TimeTools::Time OFF_TIME = { 8, 15 };

class AlarmStateMachine {
  public:
    AlarmStateMachine(shared_ptr<LightStateMachine> lightStateMachine) {
      this->lightStateMachine = lightStateMachine;

      this->setState(AlarmState::State::Off);
    }

    void setCurrentTime(TimeTools::Time &time) {
      if (TimeTools::largerThan(time, OFF_TIME)) {
        this->setState(AlarmState::State::Off);
        return;
      }

      if (TimeTools::largerThan(time, ON_TIME)) {
        this->setState(AlarmState::State::On);
        return;
      }

      if (TimeTools::largerThan(time, PREPARE_TIME)) {
        this->setState(AlarmState::State::Prepare);
        return;
      }
    }

  private:
    shared_ptr<LightStateMachine> lightStateMachine;

    void setState(AlarmState::State newState) {
      if (newState != this->state) {
        this->state = newState;
        this->lightStateMachine->alarmStateChanged(this->state);
      }
    }

    AlarmState::State state = AlarmState::State::Off;
};
