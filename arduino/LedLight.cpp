#pragma once

#include "Arduino.h"

#include "../src/Color.cpp"
#include "../src/ILight.cpp"

class LedLight : public ILight {
  public:
    LedLight(int redPin, int greenPin, int bluePin) {
      this->redPin = redPin;
      this->greenPin = greenPin;
      this->bluePin = bluePin;

      pinMode(redPin, OUTPUT);
      pinMode(greenPin, OUTPUT);
      pinMode(bluePin, OUTPUT);
    }

    void set(ColorName color, float intensity) {
      int redValue = 255 * intensity;
      int greenValue = 32 * intensity;


      switch (color) {
        case ColorName::Red:
          analogWrite(this->redPin, redValue);
          analogWrite(this->greenPin, 0);
          analogWrite(this->bluePin, 0);
          break;
        case ColorName::Yellow:
          analogWrite(this->redPin, redValue);
          analogWrite(this->greenPin, greenValue);
          analogWrite(this->bluePin, 0);
          break;
        case ColorName::Green:
          analogWrite(this->redPin, 0);
          analogWrite(this->greenPin, greenValue);
          analogWrite(this->bluePin, 0);
          break;
        case ColorName::None:
          analogWrite(this->redPin, 0);
          analogWrite(this->greenPin, 0);
          analogWrite(this->bluePin, 0);
          break;
      }
    }

    void set(ColorName color) {
      this->set(color, 1.0);
    }

    void turnOff() {
      this->set(ColorName::None);
    }

  private:
    int redPin;
    int greenPin;
    int bluePin;
};
