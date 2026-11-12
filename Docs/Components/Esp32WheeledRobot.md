# Esp32WheeledRobot

## RU

`UploadClass` **Esp32WheeledRobot** — ESP32 open-loop wheeled robot.

- База: **только** `UEsp32CustomLink` (single inheritance).
- Plugin host: composition `HostAdapter` → `UArduinoPluginHost` (план §0.4; **без** MI с PluginHost).
- Протокол: Nmsdk motor hub ESP32 twin — [Hub-Protocols.md](../Hub-Protocols.md) (`nmsdk_motor_hub_esp32_v1`), кадры `0x20`/`0x21`.
- **Не** использует Waveshare UGV JSON / WaveRover.

Sample: `Bin/Configs/SpikeSamples/Hardware/11-Esp32WheeledRobot` (`MotorDriverId=wire_l298n` или `motor_shield_r3` на esp32_uno_formfactor).

Свойства drive — как у ArduinoWheeledRobot (`LeftPwm`…`ApplyMotorDriver`, fb/sense). Baud по умолчанию 115200; DTR/RTS off.

См. [WheeledRobots.md](../WheeledRobots.md).

## EN

ESP32 wheeled robot on Nmsdk motor hub. Single inheritance + composed plugin host. No WaveRover/Waveshare coupling.
