#pragma once

#include "Color.cpp"

using namespace Color;
class ILight {
  public:
    virtual void set(ColorName color) = 0;
    virtual void set(ColorName color, float intensity) = 0;
    virtual void turnOff() = 0;
};
