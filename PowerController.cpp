#include "PowerController.h"
#include "config.h"

PowerController::PowerController() {}

void PowerController::begin() {
  pinMode(PWM_PIN_A, OUTPUT);
  pinMode(PWM_PIN_B, OUTPUT);
  pinMode(PWM_PIN_C, OUTPUT);
  pinMode(FAN_PIN_A, OUTPUT);
  pinMode(FAN_PIN_B, OUTPUT);
  pinMode(FAN_PIN_C, OUTPUT);
  pinMode(FAN_PIN_D, OUTPUT);
  // start off
  analogWrite(PWM_PIN_A, 0);
  analogWrite(PWM_PIN_B, 0);
  analogWrite(PWM_PIN_C, 0);
  digitalWrite(FAN_PIN_A, HIGH); // on
  digitalWrite(FAN_PIN_B, HIGH); // on
  digitalWrite(FAN_PIN_C, HIGH); // on
  digitalWrite(FAN_PIN_D, HIGH); // on
}

void PowerController::activateFans(float overTemp)
{
  float onPercent = 0.0f;
  if (overTemp > 0.0f)
  {
    // 0.1 degrees over -> 10%: that implies mapping 0.1 deg -> 10%; 1.0 deg -> 100%
    // So onPercent = min( (over / 1.0) * 100, 100) but we want 0.1->10 -> so scaling is *100/1.0
    // That equals over * 100. But then 0.1*100 = 10, 1.0*100 = 100. Good.
    onPercent = overTemp * 100.0f;
    if (onPercent > 100.0f)
      onPercent = 100.0f;
  }

  Serial.print("  Over: ");
  Serial.print(overTemp, 3);
  Serial.print("  Power %: ");
  Serial.print(onPercent, 1);

  setFanPinOuts(onPercent);
}

void PowerController::setFanPinOuts(float percent) {
  // percent may be negative; map to 0..100
  if (isnan(percent)) percent = 100.0f;

  if (percent <= 0) {
    setFanPins(0);
  } else {
    setFanPins(1);
  }

  float p = constrain(percent, 0.0f, 100.0f);
  uint8_t pwm = (uint8_t)round((p / 100.0f) * 255.0f);
  setPwmForAll(pwm);
}

void PowerController::setFanPins(uint8_t value) {
  Serial.print("  Fans set "); Serial.print(value);
  digitalWrite(FAN_PIN_A, value);
  digitalWrite(FAN_PIN_B, value);
  digitalWrite(FAN_PIN_C, value);
  digitalWrite(FAN_PIN_D, value);
}


void PowerController::setPwmForAll(uint8_t pwmValue) {
  Serial.print("  PWM set "); Serial.print(pwmValue);
  analogWrite(PWM_PIN_A, pwmValue);
  analogWrite(PWM_PIN_B, pwmValue);
  analogWrite(PWM_PIN_C, pwmValue);
}
