#include "PowerController.h"
#include "config.h"

PowerController::PowerController() {}

void PowerController::begin() {
  pinMode(EXTRACTOR_PWM_PIN, OUTPUT);
  pinMode(BUZZER_PWM_PIN, OUTPUT);
  pinMode(PWM_PIN_C, OUTPUT);
  pinMode(EXTRACTOR_FAN, OUTPUT);
  pinMode(SUPPLY_FAN, OUTPUT);
  pinMode(LIGHT_PIN_C, OUTPUT);
  // start off
  analogWrite(EXTRACTOR_PWM_PIN, 0);
  analogWrite(BUZZER_PWM_PIN, 0);
  analogWrite(PWM_PIN_C, 0);
  digitalWrite(EXTRACTOR_FAN, 0); // off
  digitalWrite(SUPPLY_FAN, 0); // off
  digitalWrite(LIGHT_PIN_C, 0); // off
}

void PowerController::activateFans(float overTemp)
{
  float onPercent = 0.0f;

  Serial.print("  Over: ");
  Serial.print(overTemp, 3);
  Serial.print(" || ");

  digitalWrite(EXTRACTOR_FAN, 0); // extractor off
  digitalWrite(SUPPLY_FAN, 0);    // supply off
  setPwm(0);                      // PWM power low
  tone(BUZZER_PWM_PIN, 0);        // buzzer off

  if (overTemp < 0.0f) {
    digitalWrite(LIGHT_PIN_C, 1);
    Serial.print(" Ext: off  Spl: off ");
    
  } else if (overTemp < 0.5f) {
    digitalWrite(LIGHT_PIN_C, 1);
    digitalWrite(EXTRACTOR_FAN, 1);
    Serial.print(" Ext:  ON  Spl: off ");
    
  } else if (overTemp < 1.0f) {
    digitalWrite(LIGHT_PIN_C, 1);
    digitalWrite(EXTRACTOR_FAN, 1);
    digitalWrite(SUPPLY_FAN, 1);
    Serial.print(" Ext:  ON  Spl:  ON ");
    
  } else if (overTemp < 2.0f) {
    digitalWrite(LIGHT_PIN_C, 1);
    digitalWrite(EXTRACTOR_FAN, 1);
    digitalWrite(SUPPLY_FAN, 1);
    Serial.print(" Ext:  ON  Spl:  ON ");
    
    onPercent = (overTemp - 1.0f) * 200.0f;
    onPercent = constrain(onPercent, 0.0f, 100.0f);
    setPwm(onPercent);
    
  } else if (overTemp >= 2.0f) {
    digitalWrite(LIGHT_PIN_C, 0); // off
    digitalWrite(EXTRACTOR_FAN, 1);
    digitalWrite(SUPPLY_FAN, 1);
    setPwm(100);
    tone(BUZZER_PWM_PIN, 300);
    Serial.print(" Ext:  ON  Spl:  ON ");
    Serial.println(" OVER TEMP  Light OFF");
    
  } else {
    digitalWrite(LIGHT_PIN_C, 0); // off
    tone(BUZZER_PWM_PIN, 1000);
    Serial.println(" Invalid overTemp value");
    Serial.println(" Light OFF ");
  }
}


void PowerController::setPwm(float percent) {
  if (isnan(percent)) percent = 0.0f;
  float p = constrain(percent, 0.0f, 100.0f);
  uint8_t pwm = (uint8_t)round((p / 100.0f) * 255.0f);
  Serial.print(" || PWM "); Serial.print(pwm);
  Serial.print("   Power: "); Serial.print(p, 1); Serial.print("%");
  analogWrite(EXTRACTOR_PWM_PIN, pwm);
}
