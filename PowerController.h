#ifndef POWERCONTROLLER_H
#define POWERCONTROLLER_H

#include <Arduino.h>

class PowerController {
public:
  PowerController();
  void begin();

  // Set power percent 0.0 - 100.0 (will be constrained)
  void setPowerPercent(float percent);

  // Set PWM individually
  void setPwmForAll(uint8_t pwmValue);

private:
  uint8_t _pinA, _pinB, _pinC;
};

#endif
