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

string display(lightState state) {
  switch (state) {
    case off:
      return "-        ";
    case redFull:
      return "Red      ";
    case redHalf:
      return "Red (1/2)";
    case green:
      return "Green    ";
    case yellow:
      return "Yellow   ";
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

  while (running) {

    char timeString[6];
    sprintf(timeString, "%02d:%02d", hour, minute);

    int currentPosition = 1;
    gui.print(timeString, 0);
    gui.print(display(lightController->state), currentPosition);

    usleep(500000);

    bumpTime(1);

    currentTime.hour = hour;
    currentTime.minute = minute;
    alarmStateMachine.tick(currentTime);
    lightStateMachine->tick(currentTime);

    char c = getch();

    switch (c) {
      case 'q':
        running = false;
        break;
      case 'z':
        // stateMachine.buttonLongPress();
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

  }
}
