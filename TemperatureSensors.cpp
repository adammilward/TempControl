#include "TemperatureSensors.h"

// DHT type for DHT11
#ifndef DHTTYPE
#define DHTTYPE DHT11
#endif

TemperatureSensors::TemperatureSensors(uint8_t oneWirePin)
  : _oneWirePin(oneWirePin)
{
  _oneWire = new OneWire(_oneWirePin);
  _dallas = new DallasTemperature(_oneWire);
  memset(_addresses, 0, sizeof(_addresses));
  for (uint8_t i = 0; i < SENSOR_COUNT; ++i) _factors[i] = 1.0f;
}

TemperatureSensors::~TemperatureSensors() {
  if (_dallas) {
    delete _dallas;
    _dallas = nullptr;
  }
  if (_oneWire) {
    delete _oneWire;
    _oneWire = nullptr;
  }
}

void TemperatureSensors::begin() {
  _dallas->begin();
  // discover addresses up to sensorsCount
  uint8_t found = 0;
  DeviceAddress addr;
  _dallas->setWaitForConversion(true); // we'll wait manually

  // iterate over devices on the bus and store up to _sensorsCount addresses
  for (uint8_t i = 0; i < _dallas->getDeviceCount() && found < SENSOR_COUNT; ++i) {
    if (_dallas->getAddress(addr, i)) {
      memcpy(_addresses + (found * 8), addr, 8);
      found++;
    }
  }
  if (found == 0) {
    Serial.println("Warning: no OneWire devices found in begin()");
  } else if (found < SENSOR_COUNT) {
    Serial.print("Warning: expected "); Serial.print(SENSOR_COUNT);
    Serial.print(" sensors but found "); Serial.println(found);
  }
  loadCalibration();
}

void TemperatureSensors::loadCalibration()
{
  // load calibration factors from EEPROM if present
  // EEPROM layout: [magic:uint32][factor0:float][factor1:float]...
  uint16_t addr = EEPROM_BASE;
  uint32_t magic = 0;
  EEPROM.get(addr, magic);
  addr += sizeof(magic);
  if (magic == EEPROM_MAGIC)
  {
    Serial.println("Loading calibration factors from EEPROM");
    for (uint8_t i = 0; i < SENSOR_COUNT; ++i)
    {
      float f = 1.0f;
      Serial.print("Reading factor for sensor "); Serial.print(i); Serial.print(" at addr "); Serial.println(addr);
      EEPROM.get(addr, f);
      addr += sizeof(f);
      _factors[i] = f;
      Serial.print("Factor[");
      Serial.print(i);
      Serial.print("] = ");
      Serial.println(_factors[i], 6);
    }
  }
  else
  {
    Serial.println("No calibration found in EEPROM; using factors=1.0");
  }
}

void TemperatureSensors::requestTemperatures() {
  if (!_dallas) return;
  _dallas->requestTemperatures();
  // DallasTemperature's request is non-blocking when setWaitForConversion(false)
  // but here we'll wait the maximum conversion time for 12-bit resolution (750ms)
  delay(800);
}

float TemperatureSensors::getTemperature(uint8_t index) {
  if (index >= SENSOR_COUNT) return NAN;
  DeviceAddress addr;
  memcpy(addr, _addresses + (index * 8), 8);
  // check for all-zero address
  bool allZero = true;
  for (uint8_t b = 0; b < 8; ++b) {
    if (addr[b] != 0) { allZero = false; break; }
  }
  if (allZero) return NAN;
  if (!_dallas) return NAN;
  float t = _dallas->getTempC(addr);
  return t;
}

float TemperatureSensors::getCalibratedTemperature(uint8_t index) {
  float t = getTemperature(index);
  // apply conversion factor if available
  if (index < SENSOR_COUNT)
  return t * _factors[index];
  return t;
}

void TemperatureSensors::calibrateAndStore() {
  // read all sensors and compute average
  float sum = 0.0f;
  uint8_t valid = 0;
  for (uint8_t i = 0; i < SENSOR_COUNT; ++i) {
    float v = getTemperature(i);
    if (!isnan(v)) {
      sum += v;
      valid++;
    }
  }
  if (valid == 0) {
    Serial.println("Calibration failed: no valid readings");
    return;
  }
  float avg = sum / (float)valid;
  Serial.print("Calibration average: "); Serial.println(avg, 3);
  // compute conversion factors so that factor * reading = average
  // i.e., factor = avg / reading
  for (uint8_t i = 0; i < SENSOR_COUNT; ++i) {
    float v = getTemperature(i);
    float f = 1.0f;
    if (!isnan(v) && v != 0.0f) f = avg / v;
    _factors[i] = f;
    Serial.print("Factor["); Serial.print(i); Serial.print("] = "); Serial.println(f, 6);
  }
  // store to EEPROM
  uint16_t addr = EEPROM_BASE;
  uint32_t magic = EEPROM_MAGIC;
  EEPROM.put(addr, magic);
  addr += sizeof(magic);
  for (uint8_t i = 0; i < SENSOR_COUNT; ++i) {
    float f = _factors[i];
    Serial.print("Storing factor for sensor ");
    Serial.print(i);
    Serial.print(" at addr ");
    Serial.println(addr);
    EEPROM.put(addr, f);
    EEPROM.get(addr, f); // verify write
    Serial.print("Verified readback: ");
    Serial.println(f, 6);

    addr += sizeof(f);
  }
  Serial.println("Calibration stored to EEPROM");
}

float TemperatureSensors::getMaxTemperature() {
  float maxT = -INFINITY;
  for (uint8_t i = 0; i < SENSOR_COUNT; ++i) {
    float t = getCalibratedTemperature(i);
    if (isnan(t)) Serial.print("NAN"); else Serial.print(t, 2);
    if (isnan(t)) continue;
    if (t > maxT) maxT = t;
    Serial.print(" ");
  }
  if (maxT == -INFINITY) return NAN;
  return maxT;
}

// float TemperatureSensors::getDHTAverageTemperature(uint8_t pinA, uint8_t pinB)
// {
//   float sum = 0.0f;
//   uint8_t valid = 0;
//   // local DHT objects
//   DHT dhtA(pinA, DHTTYPE);
//   DHT dhtB(pinB, DHTTYPE);
//   dhtA.begin();
//   dhtB.begin();
//   // read temps
//   float tA = dhtA.readTemperature();
//   if (!isnan(tA)) { sum += tA; valid++; }
//   float tB = dhtB.readTemperature();
//   if (!isnan(tB)) { sum += tB; valid++; }
//   if (valid == 0) return NAN;
//   return sum / (float)valid;
// }

// float TemperatureSensors::getDHTAverageHumidity(uint8_t pinA, uint8_t pinB)
// {
//   float sum = 0.0f;
//   uint8_t valid = 0;
//   DHT dhtA(pinA, DHTTYPE);
//   DHT dhtB(pinB, DHTTYPE);
//   dhtA.begin();
//   dhtB.begin();
//   float hA = dhtA.readHumidity();
//   if (!isnan(hA)) { sum += hA; valid++; }
//   float hB = dhtB.readHumidity();
//   if (!isnan(hB)) { sum += hB; valid++; }
//   if (valid == 0) return NAN;
//   return sum / (float)valid;
// }
