# Protocol Plugins

Static in-process plugins (`IArduinoProtocolPlugin`) registered via `registerArduinoProtocolPlugin`.

| Plugin id | Firmware | Host |
|-----------|----------|------|
| `sensor_lab_v1` | `sensor_lab_v1` | `ArduinoSensorSketch` / `ArduinoCustomFirmware` |
| `nmsdk_sensor_hub_v1` | `nmsdk_sensor_hub_v1` | `ArduinoCustomFirmware` (sample 14) |
| `nmsdk_motor_hub_v1` | `nmsdk_motor_hub_v1` | `ArduinoCustomFirmware` (sample 15) |

`ArduinoCustomFirmware`: generic host — upload + Command + FrameLog; typed IO when plugin found (`PluginBound`).

Decoder shared for sensor_lab / sensor hub frames: `USensorLabFrameDecoder` (type `0x01`).

Hub framing, line commands, watchdog: [Hub-Protocols.md](Hub-Protocols.md).

## EN

Same table. Hub details: [Hub-Protocols.md](Hub-Protocols.md).
