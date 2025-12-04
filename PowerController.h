#ifndef POWERCONTROLLER_H
#define POWERCONTROLLER_H

#include <Arduino.h>

class PowerController {
public:
  PowerController();
  void begin();

  // set fan power with given over-temperature
  void activateFans(float overTemp);



private:

  // Set power percent 0.0 - 100.0 (will be constrained)
  void setFanPinOuts(float percent);

  // Set PWM individually
  void setPwmForAll(uint8_t pwmValue);

  void setFanPins(uint8_t value);
};

#endif
