#include <Arduino.h>
#include <stdlib.h>
#include <ctype.h>
#include "TemperatureSensors.h"
#include "PowerController.h"
#include <EEPROM.h>

// this code was created by VSCode GPT-5 mini

// Hardware constants are defined in config.h (included via TemperatureSensors.h)
TemperatureSensors temps(ONE_WIRE_PIN);
PowerController power;

// User-configurable maximum temperature and defaults (from config.h)
// MAX_ALLOWED_TARGET_TEMP and DEFAULT_MAX_TARGET_TEMP come from config.h
float maxTargetTemp = DEFAULT_MAX_TARGET_TEMP;

unsigned long lastReadMillis = 0;
// READ_INTERVAL is defined in config.h

String inputBuffer = "";

// Parse and set maxTargetTemp from a C-string. Returns true on success.
bool handleSetMaxTarget(const char* s) {
  char* endptr = nullptr;
  double v = strtod(s, &endptr);
  if (endptr == s) return false; // not a number
  // skip trailing whitespace
  while (*endptr && isspace((unsigned char)*endptr)) endptr++;
  if (*endptr != '\0') return false; // trailing junk

  float newMax = (float)v;
  if (newMax < 0.0f) {
    newMax = 0.0f;
    Serial.println("maxTargetTemp set to: 0.0");
  }
  if (newMax > MAX_ALLOWED_TARGET_TEMP) {
    Serial.print("maxTargetTemp set to: ");
    Serial.println(MAX_ALLOWED_TARGET_TEMP, 2);
    newMax = MAX_ALLOWED_TARGET_TEMP;
  }
  maxTargetTemp = newMax;
  Serial.println();
  Serial.print("maxTargetTemp set to: ");
  Serial.println(maxTargetTemp, 2);
  // persist to EEPROM
  EEPROM.put(EEPROM_TARGET_ADDR, maxTargetTemp);

  return true;
}

void setup() {
  Serial.begin(115200);
  while (!Serial) { ; }
  Serial.println("TempControl starting");

  temps.begin();
  power.begin();
  pinMode(STATUS_LED_PIN, OUTPUT);

  // load saved max target temp from EEPROM if present
  float saved = 0.0f;
  EEPROM.get(EEPROM_TARGET_ADDR, saved);
  if (!isnan(saved) && saved >= 0.0f && saved <= MAX_ALLOWED_TARGET_TEMP) {
    maxTargetTemp = saved;
    Serial.print("Loaded saved maxTargetTemp: "); Serial.println(maxTargetTemp, 2);
  } else {
    maxTargetTemp = DEFAULT_MAX_TARGET_TEMP;
    Serial.print("Using default maxTargetTemp: "); Serial.println(maxTargetTemp, 2);
  }
  // initial read
  temps.requestTemperatures();
  lastReadMillis = millis();

}

void handleSerial() {
  while (Serial.available()) {
    char c = (char)Serial.read();
    Serial.print(c); // echo back
    if (c == '\n' || c == '\r') {
      if (inputBuffer.length() > 0) {
        Serial.println();
        inputBuffer.trim();
        Serial.print("Received input: ");
        Serial.println(inputBuffer);
        const char* s = inputBuffer.c_str();
        // check for calibrate command 'c' (case-insensitive)
        if ( (s[0] == 'c' || s[0] == 'C') && s[1] == '\0') {
          Serial.println("Calibrate command received: running calibration...");
          temps.calibrateAndStore();
        } else {
          // attempt to parse and set new max target temp
          if (!handleSetMaxTarget(s)) {
            Serial.println("Please input \"c\" for calibrate or a new max temperature value");
          }
        }
        inputBuffer = "";
        Serial.println("");
      }
    } else {
      inputBuffer += c;
      // limit buffer
      if (inputBuffer.length() > 32) inputBuffer.remove(0, inputBuffer.length() - 32);
    }
  }
}

void loop() {
  handleSerial();

  unsigned long now = millis();
  if (now - lastReadMillis >= READ_INTERVAL) {
    lastReadMillis = now;

    Serial.print("Temps: ");
    digitalWrite(STATUS_LED_PIN, HIGH);
    temps.requestTemperatures();
    float sensorMax = temps.getMaxTemperature();
    digitalWrite(STATUS_LED_PIN, LOW);

    if (isnan(sensorMax)) {
      Serial.println("Error: no valid temperature readings");
      power.activateFans(10.0f);
      return;
    }

    Serial.print("  Max: ");
    Serial.print(sensorMax, 2);
    Serial.print("  Target: ");
    Serial.print(maxTargetTemp, 1);

    float overTemp = sensorMax - maxTargetTemp;
    power.activateFans(overTemp);
    Serial.println();
  }
}

