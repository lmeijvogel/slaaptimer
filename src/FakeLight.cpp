#pragma once

#include <string>

#include "Color.cpp"
#include "ILight.cpp"

using namespace Color;

class FakeLight : public ILight {
  public:
    virtual void set(ColorName color) { };
    virtual void set(ColorName color, float intensity) { };

    virtual void turnOff() {}
};
