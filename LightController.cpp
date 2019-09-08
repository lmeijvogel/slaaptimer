#pragma once

#include <memory>

#include "ILight.cpp"

using namespace std;

enum lightState { off = 0, redFull, redHalf, green, yellow };

class LightController {
  public:
    LightController(ILight *pLight) {
      this->pLight = pLight;
    }

    lightState state = off;

    void setRedHalf() {
      this->pLight->setRed(0.5);
      state = redHalf;
    }

    void setRedFull() {
      this->pLight->setRed(1);
      state = redFull;
    }

    void setYellow() {
      this->pLight->setYellow();
      state = yellow;
    }

    void setGreen() {
      this->pLight->setGreen();
      state = green;
    }

    void turnOff() {
      this->pLight->turnOff();
      state = off;
    }

  private:
    ILight *pLight;
};
