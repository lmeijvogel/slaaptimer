#ifndef ILIGHT_CPP
#define ILIGHT_CPP

class ILight {
  public:
    virtual void setRed() = 0;
    virtual void setRed(float intensity) = 0;
    virtual void setGreen() = 0;
    virtual void setYellow() = 0;
    virtual void turnOff() = 0;
};
#endif
