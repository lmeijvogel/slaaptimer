#include "DS1307RTC.h"

#include "PhysicalButton.cpp"
#include "LedLight.cpp"
#include "Time.h"
#include "Display.cpp"

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

void initializeTime(int hour, int minute, int second);

void checkRtcAndGetTime();
void getTimeFromRtc();

const int MAX_COMMAND_BUFFER_SIZE = 12;
char commandBuffer[MAX_COMMAND_BUFFER_SIZE];
int commandBufferSize = 0;
bool commandEntered = false;

unsigned long lastSyncFromRtcMillis = 0;

RtcStatus::Status rtcStatus = RtcStatus::Status::Unknown;

LightController lightController(&light);
LightBlinker lightBlinker(&lightController);
LightStateMachine lightStateMachine(&lightController, &lightBlinker);
AlarmStateMachine alarmStateMachine(&lightStateMachine);
TM1637Display tm1637display(DISPLAY_CLOCK_PIN, DISPLAY_DIO_PIN);

Display display(&tm1637display);

void setup() {
  Serial.begin(9600);
  Serial.println("Started");

  pinMode(DISPLAY_DIO_PIN, OUTPUT);
  pinMode(DISPLAY_CLOCK_PIN, OUTPUT);

  if (ENABLE_RTC) {
    checkRtcAndGetTime();

    if (rtcStatus != RtcStatus::OK) {
      display.showRtcError(rtcStatus);
    }
  } else {
    Serial.println("RTC disabled, setting default time.");
    initializeTime(6, 30, 0);

    rtcStatus = RtcStatus::Status::OK;
  }

  time_t time = now();

  char timeMessage[60];
  sprintf(timeMessage, "The time is now % 2d:%02d", hour(time), minute(time));

  Serial.println(timeMessage);
  Serial.println("Send 'time nn:nn' to set the time");
}

void loop() {
  time_t currentTime = now();

  if (rtcStatus == RtcStatus::Status::Unknown) {
    checkRtcAndGetTime();
  }

  if (rtcStatus == RtcStatus::Status::OK) {
    display.showTime(currentTime);
  }

  long currentTimeMillis = millis();

  lightStateMachine.tick(currentTime);
  lightBlinker.tick(currentTimeMillis);

  int currentHour = hour(now());
  int currentMinute = minute(now());

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

void checkRtcAndGetTime() {
  if (!ENABLE_RTC) {
    return;
  }

  tmElements_t timeElements;

  bool rtcOk = RTC.read(timeElements);

  if (rtcOk) {
    rtcStatus = RtcStatus::Status::OK;

    setTime(makeTime(timeElements));
  } else {
    bool rtcPresent = RTC.chipPresent();

    if (!rtcPresent) {
      rtcStatus = RtcStatus::Status::Missing;

      Serial.println("RTC not present!");
    } else {
      rtcStatus = RtcStatus::Status::NotRunning;

      Serial.println("RTC not initialized!");
    }
  }
}

void getTimeFromRtc() {
  unsigned long minimumTimeBetweenSyncsMillis = 60l * 1000;

  unsigned long currentTimeMillis = millis();

  unsigned long timeSinceLastSyncMillis = currentTimeMillis - lastSyncFromRtcMillis;

  if (minimumTimeBetweenSyncsMillis < timeSinceLastSyncMillis) {
    Serial.println("Reading time from RTC unit");

    setTime(RTC.get());
    lastSyncFromRtcMillis = currentTimeMillis;
  }
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

      initializeTime(hour, minute, 0);
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

void initializeTime(int hour, int minute, int second) {
  setTime(hour, minute, second, 27, 9, 2019);

  if (ENABLE_RTC) {
    RTC.set(now());

    rtcStatus = RtcStatus::Status::Unknown;
  }
}

