#include "PowerController.h"
#include "config.h"

PowerController::PowerController() {}

void PowerController::begin() {
  pinMode(PWM_PIN_A, OUTPUT);
  pinMode(PWM_PIN_B, OUTPUT);
  pinMode(PWM_PIN_C, OUTPUT);
  pinMode(EXTRACTOR_FAN, OUTPUT);
  pinMode(SUPPLY_FAN, OUTPUT);
  pinMode(FAN_PIN_C, OUTPUT);
  // start off
  analogWrite(PWM_PIN_A, 0);
  analogWrite(PWM_PIN_B, 0);
  analogWrite(PWM_PIN_C, 0);
  digitalWrite(EXTRACTOR_FAN, 0); // off
  digitalWrite(SUPPLY_FAN, 0); // off
  digitalWrite(FAN_PIN_C, 0); // off
}

void PowerController::activateFans(float overTemp)
{
  float onPercent = 0.0f;
  if (overTemp >= 1.0f)
  {
    // 0.1 degrees over -> 20%: that implies mapping 0.1 deg -> 20%; 0.5 deg -> 100%
    onPercent = (overTemp - 1.0f) * 200.0f;
  }
  onPercent = constrain(onPercent, 0.0f, 100.0f);

  Serial.print("  Over: ");
  Serial.print(overTemp, 3);
  Serial.print("  Power: ");
  Serial.print(onPercent, 1);
  Serial.print("%");
  Serial.print("  Fans:");

  if (overTemp <0.0f) {
    digitalWrite(EXTRACTOR_FAN, 0);
    digitalWrite(SUPPLY_FAN, 0);
    Serial.print(" Ext: OFF  Spl: OFF ");
    setPwm(0);
  } else if (overTemp <= 0.5f) {
    digitalWrite(EXTRACTOR_FAN, 1);
    digitalWrite(SUPPLY_FAN, 0);
    Serial.print(" Ext:  ON  Spl: OFF ");
    setPwm(0);
  } else if (overTemp < 1.0f) {
    digitalWrite(EXTRACTOR_FAN, 1);
    digitalWrite(SUPPLY_FAN, 1);
    Serial.print(" Ext:  ON  Spl:  ON ");
    setPwm(0);
  } else if (overTemp >= 1.0f) {
    digitalWrite(EXTRACTOR_FAN, 1);
    digitalWrite(SUPPLY_FAN, 1);
    Serial.print(" Ext:  ON  Spl:  ON ");
    setPwm(onPercent);
  } else {
    Serial.println("  Invalid overTemp value");
  }
}


void PowerController::setPwm(uint8_t percent) {
  if (isnan(percent)) percent = 0.0f;
  float p = constrain(percent, 0.0f, 100.0f);
  uint8_t pwm = (uint8_t)round((p / 100.0f) * 255.0f);

  Serial.print(" PWM "); Serial.print(pwm);
  analogWrite(PWM_PIN_A, pwm);
}
