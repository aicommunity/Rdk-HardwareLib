# Wheeled robots (ADR overview)

## RU

### Решения

1. **Один MCU → один serial-owner** на canvas: `ArduinoWheeledRobot` / `Esp32WheeledRobot` / `WaveRover` (не два UNet на один USB).
2. **Два несовместимых протокола**: Nmsdk motor hub (framed `0x20`/`0x21`) ≠ Waveshare UGV JSON (`T:11`/`T:136`/`T:1001`).
3. **MI только на AVR**: `UArduinoWheeledRobot : UArduinoCustomLink + UArduinoPluginHost`. ESP32 wheeled — **single inheritance** `UEsp32CustomLink` + plugin host через composition (`HostAdapter`).
4. **Drive logic общая**: `UWheeledDriveLogic` (PWM/Dir, Stop, retransmit, SET PIN из каталога щитов).
5. **Каталог**: Motor Shield R3 и др. — `Catalog/shields/*.json`, `controlModel=dir_pwm` для DIR+PWM.

### Компоненты

| UploadClass | База | Протокол | Sample |
|-------------|------|----------|--------|
| `ArduinoWheeledRobot` | CustomLink + PluginHost | `nmsdk_motor_hub_v1` | `09-ArduinoWheeledRobot` |
| `Esp32WheeledRobot` | Esp32CustomLink + HostAdapter | `nmsdk_motor_hub_esp32_v1` | `11-Esp32WheeledRobot` |
| `WaveRover` | Esp32JsonLink | Waveshare JSON | `10-WaveRover` |
| `Esp32Board` | McuSerialBoard | Connect-only P0 | `12-Esp32Board` |

### Документы

- [Protocol-WaveshareUgvJson.md](Protocol-WaveshareUgvJson.md) — WaveRover UART JSON
- [Hub-Protocols.md](Hub-Protocols.md) — Nmsdk motor/sensor hub
- [MotorDrivers.md](MotorDrivers.md), [Modules-Catalog.md](Modules-Catalog.md)
- [Architecture.md](Architecture.md), [QA/Wheeled-And-Modules-QA.md](QA/Wheeled-And-Modules-QA.md)

## EN

Open-loop wheeled chassis: one serial owner per MCU; Nmsdk hub vs Waveshare JSON kept separate; ESP32 wheeled uses composition for plugin host (no MI with PluginHost).
