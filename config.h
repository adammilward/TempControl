#ifndef PROJECT_CONFIG_H
#define PROJECT_CONFIG_H

// Number of DS18B20 sensors connected on the OneWire bus
// Change this to match your hardware configuration before compiling.
#ifndef SENSOR_COUNT
#define SENSOR_COUNT 4
#endif

// Hardware pins - change as needed
#ifndef ONE_WIRE_PIN
#define ONE_WIRE_PIN 8
#endif

#ifndef PWM_PIN_A
#define PWM_PIN_A 3
#endif
#ifndef PWM_PIN_B
#define PWM_PIN_B 5
#endif
#ifndef PWM_PIN_C
#define PWM_PIN_C 6
#endif

// Status LED pin (defaults to built-in LED)
#ifndef STATUS_LED_PIN
#define STATUS_LED_PIN LED_BUILTIN
#endif

// Timing
#ifndef READ_INTERVAL
#define READ_INTERVAL (10UL * 1000UL)
#endif

// Temperature limits
#ifndef MAX_ALLOWED_TARGET_TEMP
#define MAX_ALLOWED_TARGET_TEMP 28.0f
#endif

// Default target temperature
#ifndef DEFAULT_MAX_TARGET_TEMP
#define DEFAULT_MAX_TARGET_TEMP 26.0f
#endif

// EEPROM address to store the max target temperature (choose a safe offset)


// EEPROM storage base address (magic + factors)
#ifndef EEPROM_BASE
#define EEPROM_BASE 0
#endif

#ifndef EEPROM_MAGIC
#define EEPROM_MAGIC 0x43414C42 // 'CALB'
#endif

#ifndef EEPROM_TARGET_ADDR
#define EEPROM_TARGET_ADDR (EEPROM_BASE + sizeof(EEPROM_MAGIC) + (SENSOR_COUNT * sizeof(float)))
#endif

#endif // PROJECT_CONFIG_H
