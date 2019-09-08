#pragma once
namespace TimeTools {
  struct Time {
    int hour;
    int minute;
  };

  bool largerThan(Time time, Time referenceTime) {
    if (referenceTime.hour < time.hour) {
      return true;
    }

    if (referenceTime.hour == time.hour && referenceTime.minute <= time.minute) {
      return true;
    }

    return false;
  }
}
