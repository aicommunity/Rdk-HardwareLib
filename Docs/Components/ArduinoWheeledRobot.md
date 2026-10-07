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
| `WatchdogMs` | int | host → `WATCHDOG` |
| `MotorDriverId` | string | id щита каталога |
| `ApplyMotorDriver` | bool edge | `SET PIN …` из каналов щита |
| `LeftPwmFb` / `RightPwmFb` | state | из кадра `0x20` |
| `LeftSense` / `RightSense` | state | current sense |
| `HostPluginId` | string | обычно `nmsdk_motor_hub_v1` |
| `NamedValuesJson` / `PluginBound` | state | plugin telemetry |

Плюс базовые Board edges: `Connect`, `Disconnect`, `UploadFirmware`, …

## EN

AVR wheeled robot on motor hub framed protocol. See RU table and sample 09.
