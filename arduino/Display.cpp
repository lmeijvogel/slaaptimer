#pragma once

#include "Time.h"
#include "TM1637Display.h"
#include "Arduino.h"

#include "RtcStatus.h"

const int DISPLAY_MAX_BRIGHTNESS = 15;
const int DISPLAY_MIN_BRIGHTNESS = 8; // Below this brightness, the display is off.


class Display {
  public:
    Display(TM1637Display *display)
    {
      this->display = display;
    }

    void showTime(time_t time) {
      int currentSecond = second(time);

      if (currentSecond == previouslyShownSecond) {
        return;
      }

      int displayBrightness = determineDisplayBrightness(time);

      display->setBrightness(displayBrightness);

      uint8_t colonBitMask = (currentSecond & 1) << 6;

      int currentHour = hour(time);
      int currentMinute = minute(time);

      // show hours (no leading 0, with blinking colon)
      display->showNumberDecEx(currentHour, colonBitMask, false, 2, 0);

      // show minutes (leading 0s)
      display->showNumberDec(currentMinute, true, 2, 2);

      previouslyShownSecond = currentSecond;
    }

    void showRtcError(RtcStatus::Status rtcStatus) {
      /* Segments:
       *  AA
       * F  B
       *  GG
       * E  C
       *  DD
       */

      const uint8_t C = SEG_A | SEG_F | SEG_E | SEG_D;

      const uint8_t N = SEG_A | SEG_F | SEG_B | SEG_E | SEG_C;

      const uint8_t O = SEG_A | SEG_F | SEG_E | SEG_D | SEG_C | SEG_B;
      const uint8_t P = SEG_A | SEG_F | SEG_E | SEG_G | SEG_B;
      const uint8_t S = SEG_A | SEG_F | SEG_G | SEG_C | SEG_D;
      const uint8_t T = SEG_A | SEG_F | SEG_E;

      const uint8_t dash = SEG_G;

      const uint8_t empty = 0;

      uint8_t msgUnknown[] = { dash, dash, dash, dash };
      uint8_t msgMissing[] = { N, O, empty, C };
      uint8_t msgNotRunning[] = { C, S, T, P };

      switch (rtcStatus) {
        case RtcStatus::Status::Unknown:
          Serial.println("Showing letters \"----\".");
          showLetters(msgUnknown);
          break;
        case RtcStatus::Status::Missing:
          Serial.println("Showing letters \"NO C\".");
          showLetters(msgMissing);
          break;
        case RtcStatus::Status::NotRunning:
          Serial.println("Showing letters \"COFF\".");
          showLetters(msgNotRunning);
          break;
        default:
          Serial.println("Unexpected rtcStatus!");
          break;
      }
    }

    void showLetters(uint8_t *letters) {
      display->setBrightness(10);
      display->setSegments(letters, 4, 0);
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

  private:
    TM1637Display *display;
    int previouslyShownSecond = -1;
};
