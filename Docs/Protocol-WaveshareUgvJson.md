# Protocol — Waveshare UGV JSON (WaveRover)

## RU

Только для `UWaveRover` / `UEsp32JsonLink`. **Не** смешивать с Nmsdk motor hub (`0x20`/`0x21`).

Код: `Core/Protocol/UWaveshareUgvJsonProtocol.{h,cpp}`, drive → `UWheeledDriveLogic::buildWaveshareT11Json`.

Baud по умолчанию **115200**. Open без AVR DTR/RTS pulse (через `UEsp32Board` / Esp32 session flags).

### Host → robot

| T | JSON | Смысл |
|---|------|--------|
| `11` | `{"T":11,"L":±0..255,"R":±0..255}` | PWM input (signed) |
| `1` | `{"T":1,"L":±0..1,"R":±0..1}` | normalized speed (опционально) |
| `136` | `{"T":136,"cmd":ms}` | heartbeat / watchdog |
| `130` | `{"T":130,"cmd":0|1}` | base feedback flow on/off |

### Robot → host

| T | Поля | Смысл |
|---|------|--------|
| `1001` | `L`, `R` (float) | feedback velocity / duty |

Парсер: `parseFeedbackT1001`. Retransmit T:11 с интервалом host (типично &lt;3 s) пока drive активен.

См. [WheeledRobots.md](WheeledRobots.md), [Components/WaveRover.md](Components/WaveRover.md).

## EN

Waveshare UGV UART JSON for WaveRover only. Encode T:11 / T:136 / T:130; parse T:1001. Independent from Nmsdk framed motor hub.
