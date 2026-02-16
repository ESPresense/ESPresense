# BME68x support audit (espresense-7)

Date: 2026-02-16

## Scope audited
- Firmware sensor implementation (`src/BME280.cpp/.h`, `src/main.cpp` references)
- Hardware settings UI (`ui/src/routes/hardware/+page.svelte`)
- Build dependencies (`platformio.ini`)

## Findings
1. **Current support path is BME280-only in naming and config keys**
   - Module namespace and settings keys are `BME280` (`BME280_I2c`, `BME280_I2c_Bus`).
   - MQTT topics/discovery names are `bme280_*` / `BME280 ...`.

2. **No BME68x-specific library is present**
   - `platformio.ini` includes `Adafruit BME280 Library`, but no BME680/BME68x/BSEC dependency.

3. **No gas/VOC output path exists in this module**
   - Current loop publishes only temperature/humidity/pressure.
   - There are no config keys, MQTT entities, or discovery entries for gas resistance/IAQ.

4. **UI currently advertises only BME280**
   - I2C sensor section label is `BME280 - Humidity + Temp + Pressure Sensor`.

## Implications for #1257 / #801 consolidation
- Existing path may partially work for BME68x environmental channels (T/H/P) if initialization accepts the device, but this is not explicit and cannot be confirmed from current code alone.
- Full BME68x expectations (especially gas/IAQ) are **not implemented**.
- Work should be split into:
  - **Compatibility track:** explicit BME68x detect + clarified labeling/docs for T/H/P compatibility.
  - **Feature track:** optional gas/IAQ support (requires additional library strategy and likely larger implementation).

## Suggested next implementation slice
- Minimal, low-risk: keep current sensor path but update naming/documentation/UI copy to make compatibility explicit (e.g., “BME280/BME68x (Temp/Humidity/Pressure)”), while preserving existing config keys for backward compatibility.
- Follow-up: evaluate BME68x gas channel support options and memory impact before adding dependencies.
