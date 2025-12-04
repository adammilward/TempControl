TempControl Arduino sketch

Overview:
- Reads 4 DS18B20 temperature sensors on a single OneWire bus.
- Compares the maximum temperature to a user-settable threshold `maxTemp` (default 26.0 C).
- Sets three PWM outputs proportionally to how much the maximum temperature exceeds `maxTemp`.

Wiring:
- Connect DS18B20 data wires to the pin defined by `ONE_WIRE_PIN` (default D2). Use a 4.7K pull-up resistor between data and 5V.
- PWM outputs are pins 3,5,6 by default. Connect these to MOSFET gates or driver inputs (not directly to heavy loads).

Usage:
- Open Serial Monitor at 115200 baud.
- To change the threshold, type a decimal number and press Enter (e.g., `27.5`).
- The sketch reads temperatures once per minute and updates outputs.

Notes:
- The code uses `OneWire` and `DallasTemperature` libraries. Make sure they're installed in your Arduino IDE.
- The mapping from over-temperature to power percent is: percent = (maxTempRead - maxTemp) * 100, clamped to 0..100. So 0.1 C over -> 10% power, 1.0 C over -> 100% power.
