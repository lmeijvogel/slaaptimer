#pragma once

#ifdef ARDUINO
class Logger {
  public:

    static void print(const char *message) {
      Serial.print(message);
    }

    static void print(char *message) {
      Serial.print(message);
    }

    static void println(const char *message) {
      Serial.println(message);
    }

    static void println(char *message) {
      Serial.println(message);
    }

#else

#include <iostream>
#include <stdlib.h>
#include <string>

using namespace std;

class Logger {
  public:
    static void print(const char *message) {
      cout << message;
    }

    static void print(char *message) {
      cout << message;
    }

    static void println(const char *message) {
      cout << message << endl;
    }

    static void println(char *message) {
      cout << message << endl;
    }
#endif
};
