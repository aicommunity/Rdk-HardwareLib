# ArduinoDeviceIO

## RU

Компонент `ArduinoDeviceIO` — датчик или актуатор поверх связанного `ArduinoFirmata` и Hardware Catalog (Tier A Firmata).

| Свойство | Смысл |
|----------|--------|
| `LinkedFirmataName` | имя `ArduinoFirmata` |
| `ModuleId` | id модуля каталога (`analog_joystick`, `ldr`, `soil_moisture`, `ir_line_tracker`, `relay`, …) |
| `Port` / `Channel` | метка пина или канал мотор-щита; пустой `Port` → `defaultPort` из JSON модуля |
| `Role` | 0=sensor, 1=actuator, 2=auto |
| `Value` / `ValueRaw` | выход датчика |
| `ValueIn` | вход актуатора (0..1 или raw) |
| `ApplyConfig` / `WriteOutput` / `ReadInput` | edges |
| `Continuous` | опрос Analog/DigitalSamples |

### Tier A (Firmata)

`runtime=firmata`: joystick, LDR (`ldr` / `фоторезистор_ldr_модуль`), soil (`soil_moisture` / `датчик_влажности_почвы`), line tracker, relay и др.

GUI ModuleId: группировка по `category`, badge `[runtime]`. Sample: `13-DeviceIO-Joystick`.

### Не на Standard Firmata

Модули с `runtime=hub` / `motor_hub` и timing-sensors: `dht11`, `dht22`, `ds18b20`, `hc_sr04`, … — используйте `ArduinoCustomFirmware` + [Hub-Protocols.md](../Hub-Protocols.md) (samples `14-SensorHub`, `15-I2cHub-BME280`).

См. [Shields-Firmata-IO.md](../Shields-Firmata-IO.md), [Modules-Catalog.md](../Modules-Catalog.md).

## EN

Firmata-backed catalog DeviceIO. Empty `Port` falls back to module `defaultPort`. Hub/timing modules need CustomFirmware hubs, not Standard Firmata.
