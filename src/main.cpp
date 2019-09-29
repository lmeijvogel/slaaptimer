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

int hour = 6;
int minute = 0;

void bumpTime(int numberOfMinutes) {
  minute += numberOfMinutes;

  if (minute >= 60) {
    minute -= 60;
    hour += 1;
  }

  if (hour >= 24) {
    hour -= 24;
  }
}

int main() {
  FakeLight light;
  TimeTools::Time currentTime;

  lightController = make_shared<LightController>((ILight *)&light);
  lightStateMachine = make_shared<LightStateMachine>(lightController);

  AlarmStateMachine alarmStateMachine(lightStateMachine);

  bool running = true;

  long loop = 0;

  while (running) {
    int elapsedTimeMs = loop * 500000;

    char timeString[6];
    sprintf(timeString, "%02d:%02d", hour, minute);

    int currentPosition = 1;
    gui.print(timeString, 0);
    gui.print(display(lightController), currentPosition);

    usleep(500000);

    bumpTime(1);

    currentTime.hour = hour;
    currentTime.minute = minute;

    /* How to distinguish between minute-ticks and program loop tick? */
    alarmStateMachine.setCurrentTime(currentTime);
    lightStateMachine->tick(elapsedTimeMs);

    char c = getch();

    switch (c) {
      case 'q':
        running = false;
        break;
      case 'z':
        lightStateMachine->buttonLongPress(elapsedTimeMs);
        break;
      case 's':
        bumpTime(10);
        break;
      case 'd':
        bumpTime(30);
        break;
      case 'f':
        bumpTime(120);
        break;
      case 'h':
        bumpTime(360);
        break;
      case ' ':
        lightStateMachine->buttonPressed(currentTime);
        break;
    }

    loop++;
  }
}