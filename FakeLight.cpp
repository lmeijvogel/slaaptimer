#pragma once

#include <string>

#include "ILight.cpp"

class FakeLight : public ILight {
  public:
    virtual void setRed() {}
    virtual void setRed(float intensity) {};
    virtual void setGreen() {}
    virtual void setYellow() {}

    virtual void turnOff() {}
};
