#include "catch.hpp"

#include "../RtcAdapter.cpp"


TEST_CASE("It converts received data") {
  ds1302_struct input;

  input.Seconds = 2;
  input.Seconds10 = 2;
  input.Minutes = 5;
  input.Minutes10 = 2;
  input.Hour = 3;
  input.Hour10 = 1;

  RtcAdapter rtcAdapter;

  Time rtcResult;

  rtcAdapter.parse(input, rtcResult);

  REQUIRE(rtcResult.hour == 13);
  REQUIRE(rtcResult.minute == 25);
  REQUIRE(rtcResult.second == 22);
}


