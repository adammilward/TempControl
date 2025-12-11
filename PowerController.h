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
  // Set PWM as percent 0..100 (will be constrained and converted to 0..255)
  void setPwm(uint8_t percent);
};

#endif
