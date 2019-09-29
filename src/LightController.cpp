#pragma once

#include <memory>

#include "Color.cpp"
#include "ILight.cpp"

using namespace Color;
using namespace std;

struct LightControllerState {
  bool isOn;

  ColorName color;
  float intensity;
};

class LightController {
  public:
    LightController(ILight *pLight) {
      this->pLight = pLight;
    }

    void set(ColorName color) {
      this->pLight->set(color);

      this->intensity = 1;
      this->color = color;
      this->isOn = true;
    }

    void set(ColorName color, float intensity) {
      this->pLight->set(color, intensity);

      this->intensity = intensity;
      this->color = color;
      this->isOn = true;
    }

    void setState(LightControllerState &state) {
      if (state.isOn) {
        this->set(state.color, state.intensity);
      } else {
        this->turnOff();
      }
    }

    void turnOff() {
      this->pLight->turnOff();

      this->intensity = 0;
      this->isOn = false;
      this->color = None;
    }

    void getState(LightControllerState *state) {
      state->isOn = this->isOn;
      state->color = this->color;
      state->intensity = this->intensity;
    }

    float getIntensity() {
      return this->intensity;
    }

    bool getIsOn() {
      return this->isOn;
    }

    ColorName getColor() {
      return this->color;
    }

  private:
    ILight *pLight;

    float intensity = 0;
    ColorName color = None;

    bool isOn = false;
};
