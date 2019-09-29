#include <stdlib.h>
#include <unistd.h>
#include <vector>
#include <memory>

#include "NCursesGui.cpp"
#include "ILight.cpp"
#include "FakeLight.cpp"

#include "LightController.cpp"
#include "AlarmStateMachine.cpp"
#include "LightStateMachine.cpp"

#include "lib/TimeTools.cpp"

string display(shared_ptr<LightController> lightController) {
  if (lightController->getIsOn()) {
    switch (lightController->getColor()) {
      case None:
        return "!! NONE !";
      case Red:
        if (lightController->getIntensity() < 1.0)
          return "Red (1/2)";
        else
          return "Red      ";
      case Green:
        return "Green    ";
      case Yellow:
        return "Yellow   ";
    }
  } else {
    return "-        ";
  }
}

NCursesGui gui;

shared_ptr<LightController> lightController;
shared_ptr<LightStateMachine> lightStateMachine;

int day = 0;
int hour = 6;
int minute = 0;

void bumpTime(int numberOfMinutes) {
  minute += numberOfMinutes;

  while (minute >= 60) {
    minute -= 60;
    hour += 1;
  }

  while (hour >= 24) {
    hour -= 24;

    day += 1;
  }
}

int main() {
  FakeLight light;
  TimeTools::Time currentTime;

  lightController = make_shared<LightController>((ILight *)&light);
  lightStateMachine = make_shared<LightStateMachine>(lightController);

  AlarmStateMachine alarmStateMachine(lightStateMachine);

  bool running = true;

  while (running) {
    long oneMinute = 60*1000;

    long hours = day * 24 + hour;
    long minutes = hours * 60 + minute;

    long  elapsedTimeMs = minutes * oneMinute;

    char timeString[6];
    sprintf(timeString, "%02d:%02d", hour, minute);

    int currentPosition = 1;
    gui.print(timeString, 0);
    gui.print(display(lightController), currentPosition);

    usleep(500000);

    bumpTime(1);

    currentTime.hour = hour;
    currentTime.minute = minute;

    alarmStateMachine.setCurrentTime(currentTime);
    lightStateMachine->tick(elapsedTimeMs);

    char c = getch();

    switch (c) {
      case 'q':
        running = false;
        break;
      case 'z':
        lightStateMachine->toggleAutoOff(elapsedTimeMs);
        break;
      case 'r':
        hour = 6;
        minute = 20;
        break;
      case 's':
        bumpTime(10);
        break;
      case 'd':
        bumpTime(30);
        break;
      case 'f':
        bumpTime(60);
        break;
      case 'h':
        bumpTime(120);
        break;
      case ' ': // "Button press"
        lightStateMachine->toggleLight(elapsedTimeMs);
        break;
    }
  }
}
