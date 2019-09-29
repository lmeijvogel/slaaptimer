#pragma once

#include "Arduino.h"

const int PRESS_THRESHOLD = 6;

class PhysicalButton {
public:
  PhysicalButton(int buttonId) {
    this->buttonId = buttonId;

    pinMode(buttonId, INPUT_PULLUP);
  }

  void clockTick() {
    /* Use a form of Kuhn's algorithm for debouncing buttons:
     * Basically: Keep a counter that increases if a LOW (pressed)
     * signal is received from the button, and decreases otherwise.
     *
     * Whenever a boundary value is hit (0 or PRESS_THRESHOLD), change the
     * state to false (not pressed) resp. true (pressed).
     */
    int reading = digitalRead(buttonId);

    if (reading == LOW) {
      pressIntegrator++;
    } else {
      pressIntegrator--;
    }

    if (pressIntegrator <= 0) {
      pressIntegrator = 0;

      _pressed = false;
    }

    if (pressIntegrator > PRESS_THRESHOLD) {
      _pressed = true;
      pressIntegrator = PRESS_THRESHOLD;
    }
  }

  boolean isPressed() {
    return _pressed;
  }

 private:
  int buttonId;

  int pressIntegrator = 0;

  bool _pressed = false;
};
