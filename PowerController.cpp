#include "PowerController.h"
#include "config.h"

PowerController::PowerController() {}

void PowerController::begin() {
  pinMode(PWM_PIN_A, OUTPUT);
  pinMode(PWM_PIN_B, OUTPUT);
  pinMode(PWM_PIN_C, OUTPUT);
  // start off
  analogWrite(PWM_PIN_A, 0);
  analogWrite(PWM_PIN_B, 0);
  analogWrite(PWM_PIN_C, 0);
}

void PowerController::setPowerPercent(float percent) {
  // percent may be negative; map to 0..100
  if (isnan(percent)) percent = 0.0f;
  float p = constrain(percent, 0.0f, 100.0f);
  uint8_t pwm = (uint8_t)round((p / 100.0f) * 255.0f);
  setPwmForAll(pwm);
}

void PowerController::setPwmForAll(uint8_t pwmValue) {
  analogWrite(PWM_PIN_A, pwmValue);
  analogWrite(PWM_PIN_B, pwmValue);
  analogWrite(PWM_PIN_C, pwmValue);
}
