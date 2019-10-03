#include "PhysicalButton.cpp"
#include "LedLight.cpp"
#include "Time.h"
#include "Display.cpp"
#include "RtcClock.cpp"

#include "../src/LightController.cpp"
#include "../src/LightStateMachine.cpp"
#include "../src/AlarmStateMachine.cpp"

#include "RtcStatus.h"

/* SCL: A4
 * SDA: A5
 */

const int BUTTON_PIN = 3;
const int RED_PIN = 9;
const int GREEN_PIN = 10;
const int BLUE_PIN = 11;

const bool ENABLE_RTC = false;

const long LONG_PRESS_THRESHOLD_MILLIS = 1000;

LedLight light(RED_PIN, GREEN_PIN, BLUE_PIN);
PhysicalButton button(BUTTON_PIN);

void checkButtonPress();

bool buttonWasPressed = false;
long buttonPressStartMillis = 0;
bool longPressRegistered = false;
int state = 0;

void checkSerialCommand();

void parseCommandBuffer();

const int MAX_COMMAND_BUFFER_SIZE = 12;
char commandBuffer[MAX_COMMAND_BUFFER_SIZE];
int commandBufferSize = 0;
bool commandEntered = false;

unsigned long lastSyncFromRtcMillis = 0;

LightController lightController(&light);
LightBlinker lightBlinker(&lightController);
LightStateMachine lightStateMachine(&lightController, &lightBlinker);
AlarmStateMachine alarmStateMachine(&lightStateMachine);

TM1637Display tm1637display(DISPLAY_CLOCK_PIN, DISPLAY_DIO_PIN);
Display display(&tm1637display);

RtcClock rtcClock(ENABLE_RTC);

void setup() {
  Serial.begin(9600);
  Serial.println("Started");

  pinMode(DISPLAY_DIO_PIN, OUTPUT);
  pinMode(DISPLAY_CLOCK_PIN, OUTPUT);

  if (ENABLE_RTC) {
    rtcClock.checkRtcAndGetTime();

    if (rtcClock.status() != RtcStatus::OK) {
      display.showRtcError(rtcClock.status());
    }
  } else {
    Serial.println("RTC disabled, setting default time.");
    rtcClock.initializeTime(6, 30, 0);
  }

  time_t time = now();

  char timeMessage[60];
  sprintf(timeMessage, "The time is now % 2d:%02d", hour(time), minute(time));

  Serial.println(timeMessage);
  Serial.println("Send 'time nn:nn' to set the time");
}

void loop() {
  if (rtcClock.status() == RtcStatus::Status::Unknown) {
    rtcClock.checkRtcAndGetTime();
  }

  time_t currentTime = now();

  if (rtcClock.status() == RtcStatus::Status::OK) {
    display.showTime(currentTime);
  }

  lightStateMachine.tick(currentTime);
  lightBlinker.tick(millis());

  int currentHour = hour(currentTime);
  int currentMinute = minute(currentTime);

  alarmStateMachine.setCurrentTime(currentHour, currentMinute);

  checkButtonPress();

  checkSerialCommand();
}

void checkButtonPress() {
  button.clockTick();

  bool buttonIsPressed = button.isPressed();

  time_t currentTime = now();
  long currentTimeMillis = millis();

  if (!buttonWasPressed && buttonIsPressed) {
    buttonPressStartMillis = currentTimeMillis;
  }

  if (buttonWasPressed) {
    long pressDuration = currentTimeMillis - buttonPressStartMillis;

    bool wasLongPress = LONG_PRESS_THRESHOLD_MILLIS < pressDuration;

    bool stillPressing = buttonIsPressed;

    if (stillPressing) {
      if (wasLongPress && !longPressRegistered) {
        longPressRegistered = true;
        Serial.println("Button long press");

        lightStateMachine.toggleAutoOff(currentTime, currentTimeMillis);
      }
    }

    if (!stillPressing) {
      longPressRegistered = false;
    }

    if (!stillPressing && !wasLongPress) {
      Serial.println("Button short press");

      lightStateMachine.toggleLight(currentTime);
    }
  }

  buttonWasPressed = buttonIsPressed;
}

void parseCommandBuffer() {
  Serial.print("Received: ");
  Serial.println(commandBuffer);

  int hour;
  int minute;

  int parametersMatched = sscanf(commandBuffer, "time %d:%d", &hour, &minute);

  if (parametersMatched == 2) {
    char message[60];

    if (0 <= hour && hour < 24 && 0 <= minute && minute < 60) {
      sprintf(message, "Setting time to %d:%02d", hour, minute);

      Serial.println(message);

      rtcClock.initializeTime(hour, minute, 0);
    } else {
      // Use fact that the prefix is 'time' to build the sentence
      sprintf(message, "Invalid %s", commandBuffer);

      Serial.println(message);
    }
  } else {
    Serial.print("Invalid command: ");
    Serial.println(commandBuffer);
  }

  commandEntered = false;
  commandBufferSize = 0;
}

void checkSerialCommand() {
  while (Serial.available()) {
    if (MAX_COMMAND_BUFFER_SIZE <= commandBufferSize) {
      // Throw away command
      commandBuffer[0] = '\0';
      commandBufferSize = 0;

      Serial.println("Too many chars!");
      return;
    }

    char input = Serial.read();

    if (input == '\r') {
      commandBuffer[commandBufferSize++] = '\0';

      parseCommandBuffer();
      commandEntered = true;
    } else {
      commandBuffer[commandBufferSize++] = input;
    }
  }
}
