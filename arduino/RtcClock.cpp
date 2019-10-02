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
      if (!enableRtc) {
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

    void initializeTime(int hour, int minute, int second) {
      setTime(hour, minute, second, 27, 9, 2019);

      if (enableRtc) {
        RTC.set(now());

        rtcStatus = RtcStatus::Status::Unknown;
      }
    }

  private:
    bool enableRtc;
    RtcStatus::Status rtcStatus;
};
