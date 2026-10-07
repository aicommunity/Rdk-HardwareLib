# WaveRover

## RU

`UploadClass` **WaveRover** — stock Waveshare UGV JSON over UART (`UEsp32JsonLink`).

Протокол: [Protocol-WaveshareUgvJson.md](../Protocol-WaveshareUgvJson.md) (`T:11`, `T:136`, feedback `T:1001`).

Каталог платы: `Catalog/boards/wave_rover.json`. Flash stock GD — Waveshare tools (не avrdude).

Sample: `Bin/Configs/SpikeSamples/Hardware/10-WaveRover` (baud **115200**, DTR/RTS off).

### Properties

| Свойство | Тип | Роль |
|----------|-----|------|
| `LeftPwm` / `RightPwm` | int | magnitude 0..255 |
| `LeftDir` / `RightDir` | int | знак для T:11 |
| `ApplyDrive` / `Stop` | bool edge | TX T:11 / stop |
| `WatchdogMs` | int | T:136 timeout |
| `LastFeedbackJson` | state | last RX line (GUI Feedback tab) |

`MotorDriverId` в GUI скрыт/disabled (не hub SET PIN).

Ссылка на upstream SDK допускается **только** в этом файле (`/home/user/WaveRover`).

## EN

WaveRover stock JSON UART client. See Protocol-WaveshareUgvJson.md and sample 10.
