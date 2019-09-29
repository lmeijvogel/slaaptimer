#include "TM1637Display.h"
#include "DS1307RTC.h"

#include "PhysicalButton.cpp"
#include "LedLight.cpp"
#include "Time.h"

#include "../src/LightController.cpp"
#include "../src/LightStateMachine.cpp"
#include "../src/AlarmStateMachine.cpp"

/* SDA: A5
 * SCL: A4
 */
const int BUTTON_PIN = 3;
const int RED_PIN = 9;
const int GREEN_PIN = 10;
const int BLUE_PIN = 11;

const int DISPLAY_CLOCK_PIN = 7;
const int DISPLAY_DIO_PIN = 6;

const int DISPLAY_MAX_BRIGHTNESS = 15;
const int DISPLAY_MIN_BRIGHTNESS = 8; // Below this brightness, the display is off.

const bool ENABLE_RTC = false;

TM1637Display display(DISPLAY_CLOCK_PIN, DISPLAY_DIO_PIN);
LedLight light(RED_PIN, GREEN_PIN, BLUE_PIN);
PhysicalButton button(BUTTON_PIN);

void checkButtonPress();
bool buttonWasPressed = false;
int state = 0;

void nextState();
void applyState();

void checkSerialCommand();

void parseCommandBuffer();

void showTime(time_t time);

int determineDisplayBrightness(time_t time);
void initializeTime(int hour, int minute, int second);

void checkRtcAndGetTime();
void getTimeFromRtc();

const int MAX_COMMAND_BUFFER_SIZE = 12;
char commandBuffer[MAX_COMMAND_BUFFER_SIZE];
int commandBufferSize = 0;
bool commandEntered = false;

unsigned long lastSyncFromRtcMillis = 0;
bool rtcPresent;
bool rtcRunning;

void setup() {
  Serial.begin(9600);
  Serial.println("Started");

  if (ENABLE_RTC) {
    checkRtcAndGetTime();
  } else {
    Serial.println("RTC disabled, setting default time.");
    initializeTime(6, 30, 0);
  }

  time_t time = now();

  char timeMessage[60];
  sprintf(timeMessage, "The time is now % 2d:%02d", hour(time), minute(time));

  Serial.println(timeMessage);
  Serial.println("Send 'time nn:nn' to set the time");

  pinMode(DISPLAY_DIO_PIN, OUTPUT);
  pinMode(DISPLAY_CLOCK_PIN, OUTPUT);

  applyState();
}

void loop() {
  checkButtonPress();

  checkSerialCommand();

  showTime(now());
}

void checkButtonPress() {
  button.clockTick();

  bool buttonIsPressed = button.isPressed();

  if (buttonWasPressed && !buttonIsPressed) {
    Serial.println("Button pressed!");
    nextState();

    Serial.print("New state: ");
    Serial.println(state);
    applyState();
  }

  buttonWasPressed = buttonIsPressed;
}

void checkRtcAndGetTime() {
  tmElements_t timeElements;

  bool readStatus = RTC.read(timeElements);

  if (readStatus) {
    setTime(makeTime(timeElements));

    rtcPresent = true;
    rtcRunning = true;
  } else {
    rtcPresent = RTC.chipPresent();

    if (!rtcPresent) {
      Serial.println("RTC not present!");
      rtcRunning = false;

      return;
    } else {
      Serial.println("RTC not initialized!");
      rtcRunning = false;
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

void showTime(time_t time) {
  int displayBrightness = determineDisplayBrightness(time);

  display.setBrightness(displayBrightness);

  int currentHour = hour(time);
  int currentMinute = minute(time);

  uint8_t colonBitMask = (second(time) & 1) << 6;

  // show hours (no leading 0, with blinking colon)
  display.showNumberDecEx(currentHour, colonBitMask, false, 2, 0);

  // show minutes (leading 0s)
  display.showNumberDec(currentMinute, true, 2, 2);
}

int determineDisplayBrightness(time_t time) {
  int low = 8;
  int high = 12;

  int currentHour = hour(time);

  if (currentHour < 7 || 19 <= currentHour) {
    return low;
  } else {
    return high;
  }
}

void nextState() {
  state++;
  state &= 0x3;
}

void applyState() {
  switch (state) {
    case 0:
      light.set(ColorName::None);
      break;
    case 1:
      light.set(ColorName::Red);
      break;
    case 2:
      light.set(ColorName::Yellow);
      break;
    case 3:
      light.set(ColorName::Green);
      break;
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
      return;
    }

    char input = Serial.read();

    if (input == '\r' || input == '\n') {
      input = '\0';

      parseCommandBuffer();
      commandEntered = true;
    }

    commandBuffer[commandBufferSize++] = input;
  }
}

void initializeTime(int hour, int minute, int second) {
  setTime(hour, minute, second, 27, 9, 2019);

  RTC.set(now());
}
