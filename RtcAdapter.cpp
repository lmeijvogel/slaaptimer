#pragma once

#define bcd2bin(h,l)    (((h)*10) + (l))
#define bin2bcd_h(x)   ((x)/10)
#define bin2bcd_l(x)    ((x)%10)

// Structure for the first 8 registers.
// These 8 bytes can be read at once with
// the 'clock burst' command.
struct ds1302_struct
{
  uint8_t Seconds:4;      // low decimal digit 0-9
  uint8_t Seconds10:3;    // high decimal digit 0-5
  uint8_t CH:1;           // CH = Clock Halt
  uint8_t Minutes:4;
  uint8_t Minutes10:3;
  uint8_t reserved1:1;
  uint8_t Hour:4;
  uint8_t Hour10:2;
  uint8_t reserved2:1;
  uint8_t hour_12_24:1; // 0 for 24 hour format
  uint8_t Date:4;           // Day of month, 1 = first day
  uint8_t Date10:2;
  uint8_t reserved3:2;
  uint8_t Month:4;          // Month, 1 = January
  uint8_t Month10:1;
  uint8_t reserved4:3;
  uint8_t Day:3;            // Day of week, 1 = first day (any day)
  uint8_t reserved5:5;
  uint8_t Year:4;           // Year, 0 = year 2000
  uint8_t Year10:4;
  uint8_t reserved6:7;
  uint8_t WP:1;             // WP = Write Protect
};

struct Time {
  uint8_t hour;
  uint8_t minute;
  uint8_t second;
};

class RtcAdapter {
  public:
    void parse(ds1302_struct input, Time &output) {
      output.hour = bcd2bin(input.Hour10, input.Hour);
      output.minute = bcd2bin(input.Minutes10, input.Minutes);
      output.second = bcd2bin(input.Seconds10, input.Seconds);
    }
};
