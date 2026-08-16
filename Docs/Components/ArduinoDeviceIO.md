# ArduinoDeviceIO

Компонент `ArduinoDeviceIO` — датчик или актуатор поверх связанного `ArduinoFirmata` и Hardware Catalog.

| Свойство | Смысл |
|----------|--------|
| `LinkedFirmataName` | имя `ArduinoFirmata` |
| `ModuleId` | id модуля каталога (`potentiometer`, `servo`, `dc_motor_channel`, …) |
| `Port` / `Channel` | метка пина или канал мотор-щита |
| `Role` | 0=sensor, 1=actuator, 2=auto |
| `Value` / `ValueRaw` | выход датчика |
| `ValueIn` | вход актуатора (0..1 или raw) |
| `ApplyConfig` / `WriteOutput` / `ReadInput` | edges |
| `Continuous` | опрос Analog/DigitalSamples |

**Не поддерживается на Standard Firmata:** `dht11`, `hc_sr04` (нужен hub, фаза 6).

См. [Shields-Firmata-IO.md](../Shields-Firmata-IO.md).
