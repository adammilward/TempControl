#ifndef TEMPERATURESENSORS_H
#define TEMPERATURESENSORS_H

#include <Arduino.h>
#include <OneWire.h>
#include <DallasTemperature.h>
#include <EEPROM.h>
// DHT sensor
#include <DHT.h>

#include "config.h"

class TemperatureSensors {
public:
  // constructor: oneWirePin - pin for the OneWire bus
  TemperatureSensors(uint8_t oneWirePin);

  // destructor: free resources
  ~TemperatureSensors();

  void begin();

  void loadCalibration();

  // Request a temperature conversion and wait for completion
  void requestTemperatures();

  // Get temperature for sensor index (0..sensorsCount-1). Returns NAN if out of range.
  float getTemperature(uint8_t index);

  // Get maximum of all read temperatures
  float getMaxTemperature();

  // Get calibrated temperature for sensor index (0..sensorsCount-1). Returns NAN if out of range.
  float getCalibratedTemperature(uint8_t index);

  // Number of sensors expected (compile-time)
  uint8_t getSensorCount() const { return (uint8_t)SENSOR_COUNT; }

  // perform calibration: compute conversion factors and store to EEPROM
  void calibrateAndStore();

  // Read and return average temperature from two DHT11 modules (pins provided).
  // Returns NAN if both readings invalid.
  float getDHTAverageTemperature(uint8_t pinA, uint8_t pinB);

  // Read and return average humidity from two DHT11 modules (pins provided).
  // Returns NAN if both readings invalid.
  float getDHTAverageHumidity(uint8_t pinA, uint8_t pinB);

  // non-copyable (shallow copies would break ownership)
  TemperatureSensors(const TemperatureSensors&) = delete;
  TemperatureSensors& operator=(const TemperatureSensors&) = delete;

private:
  uint8_t _oneWirePin;
  OneWire* _oneWire;
  DallasTemperature* _dallas;
  // flat buffer: _addresses holds SENSOR_COUNT*8 bytes (8 bytes per DeviceAddress)
  uint8_t _addresses[8 * SENSOR_COUNT]; // fixed array to store found addresses (flat)
  float _factors[SENSOR_COUNT]; // conversion factors per sensor
};

#endif
