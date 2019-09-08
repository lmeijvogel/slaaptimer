/*
 * The AlarmClock waits for a given time and then performs the configured Command.
 *
 * All Commands should schedule a next command (even if it's a no-op)
 *
 * There will always be only one AlarmClock that is updated with the next ZZ
 */

#pragma once

class AlarmClock {
  public:
    AlarmClock(int initialHour, int initialMinute, shared_ptr<Command> initialCommand) {
      this->hour = initialHour;
      this->minute = initialMinute;
      this->command = initialCommand;
    }

    void tick(int currentHour, int currentMinute) {
      if (performed) {
        return;
      }

      if (hour == currentHour && minute < currentMinute) {
        performed = true;
        command->call();
      }
    }

  private:
    int hour;
    int minute;

    bool performed = false;

    shared_ptr<Command> command;
}
