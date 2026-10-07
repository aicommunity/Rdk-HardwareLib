# Esp32Board

## RU

Компонент `UEsp32Board` (`UploadClass` **Esp32Board**): serial-плата ESP32 на базе `UMcuSerialBoard`.

- Baud по умолчанию **115200**
- При open: DTR/RTS off (без AVR reset-pulse)
- Параметры: `ChipTarget`, `FlashBaud`
- UploadFirmware в P0 — stub (LastError с подсказкой esptool); Connect/Disconnect/heartbeat — рабочие

См. SpikeSample `Bin/Configs/SpikeSamples/Hardware/12-Esp32Board`.

## EN

`UEsp32Board` serial MCU board for ESP32. Default baud 115200; no DTR toggle on open. Firmware flash via IDE esptool is P1; P0 is Connect-only.
