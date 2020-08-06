#pragma once

#include "Arduino.h"
#include "DS1307RTC.h"
#include "RtcStatus.h"

class RtcClock {
  public:
    RtcClock(bool enableRtc) {
      this->enableRtc = enableRtc;
    }

    void checkRtcAndGetTime() {
      this->checkRtcAndGetTime(true);
    }

    void checkRtcAndGetTimeUnlessErroneous() {
      this->checkRtcAndGetTime(false);
    }

    void initializeTime(int hour, int minute, int second) {
      setTime(hour, minute, second, 27, 9, 2019);

      if (enableRtc) {
        RTC.set(now());

        rtcStatus = RtcStatus::Status::Unknown;
      }
    }

    RtcStatus::Status status() {
      return this->rtcStatus;
    }


  private:
    void checkRtcAndGetTime(bool force) {
      if (!enableRtc) {
        return;
      }

      tmElements_t timeElements;

      bool rtcOk = RTC.read(timeElements);

      if (rtcOk) {
        rtcStatus = RtcStatus::Status::OK;

        // Do not change the current time if it's obviously wrong:
        // I think sometimes the connection to the RTC is broken,
        // which results in the clock being badly off (as in: more than one hour).
        if (force || closeEnoughToSystemTime(timeElements)) {
          setTime(makeTime(timeElements));
        }
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

    bool closeEnoughToSystemTime(tmElements_t &timeElements) {
      time_t currentTime = now();

      return (timeElements.Hour == hour(currentTime) && timeElements.Minute == minute(currentTime));
    }

    bool enableRtc;
    RtcStatus::Status rtcStatus;
};
