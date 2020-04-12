#pragma once

#include "LightStateMachine.cpp"
#include "AlarmState.cpp"
#include "Logger.cpp"

struct AlarmTime {
  int hour;
  int minute;
};

const AlarmTime PREPARE_TIME = { 6, 30 };
const AlarmTime ON_TIME = { 7, 0 };
const AlarmTime OFF_TIME = { 8, 0 };

class AlarmStateMachine {
  public:
    AlarmStateMachine(LightStateMachine *lightStateMachine) {
      this->lightStateMachine = lightStateMachine;

      this->setState(AlarmState::State::Off);
    }

    void setCurrentTime(int hour, int minute) {
      AlarmTime time = { hour, minute };

      if (largerThan(time, OFF_TIME)) {
        this->setState(AlarmState::State::Off);
        return;
      }

      if (largerThan(time, ON_TIME)) {
        this->setState(AlarmState::State::On);
        return;
      }

      if (largerThan(time, PREPARE_TIME)) {
        this->setState(AlarmState::State::Prepare);
        return;
      }
    }

  private:
    LightStateMachine *lightStateMachine;

    void setState(AlarmState::State newState) {
      if (newState != this->state) {
        this->state = newState;
        this->lightStateMachine->alarmStateChanged(this->state);
      }
    }


    bool largerThan(AlarmTime time, AlarmTime referenceTime) {
      return toMinutes(time) > toMinutes(referenceTime);
    }

    AlarmState::State state = AlarmState::State::Off;

    int toMinutes(AlarmTime alarmTime) {
      return (alarmTime.hour * 60) + alarmTime.minute;
    }
};
