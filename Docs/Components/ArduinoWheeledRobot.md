# ArduinoWheeledRobot

## RU

`UploadClass` **ArduinoWheeledRobot** — AVR open-loop wheeled robot (`UArduinoCustomLink` + `UArduinoPluginHost`).

Протокол: [Hub-Protocols.md](../Hub-Protocols.md) (`nmsdk_motor_hub_v1`). Обзор: [WheeledRobots.md](../WheeledRobots.md).

Sample: `Bin/Configs/SpikeSamples/Hardware/09-ArduinoWheeledRobot` (`MotorDriverId=motor_shield_r3`, пины A: D12/D3, B: D13/D11).

### Drive / motor properties

| Свойство | Тип | Роль |
|----------|-----|------|
| `LeftPwm` / `RightPwm` | int | 0..255 |
| `LeftDir` / `RightDir` | int | 0/1 |
| `ApplyDrive` | bool edge | отправить MOTOR A/B |
| `Stop` | bool edge | `MOTOR STOP` |
| `WatchdogMs` | int | host → `WATCHDOG`; clamped to 500–60000 ms and resent when changed |
| `MotorDriverId` | string | id щита каталога |
| `ApplyMotorDriver` | bool edge | `SET PIN …` из каналов щита |
| `LeftPwmFb` / `RightPwmFb` | state | из кадра `0x20` |
| `LeftSense` / `RightSense` | state | current sense |
| `HostPluginId` | string | обычно `nmsdk_motor_hub_v1` |
| `NamedValuesJson` / `PluginBound` | state | plugin telemetry |

Плюс базовые Board edges: `Connect`, `Disconnect`, `UploadFirmware`, …

`HeartbeatIntervalMs` defaults to 500 ms so `PING` refreshes the firmware's default 2000 ms watchdog before it expires. The host can change `WatchdogMs`; zero is clamped to 500 ms so the motor fail-safe stays enabled.

`ApplyMotorDriver` loads each channel's `dir`, `dir2`, `pwm`, `enable`, `brake` and `sense` pins from the catalog. Bundled mappings cover Motor Shield R3, Seeed Motor Shield V1 and the Uno/Mega L298N profile.

## EN

AVR wheeled robot on motor hub framed protocol. The default health check sends `PING` every 500 ms against the 2000 ms firmware watchdog; `WatchdogMs` is clamped to 500–60000 ms. Driver setup supports the R3 DIR/PWM profile and two-direction-pin drivers. See RU table and sample 09.
