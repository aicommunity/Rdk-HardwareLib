# Protocol Plugins

Static in-process plugins (`IArduinoProtocolPlugin`) registered via `registerArduinoProtocolPlugin`.

| Plugin id | Firmware | Host |
|-----------|----------|------|
| `sensor_lab_v1` | `sensor_lab_v1` | `ArduinoSensorSketch` / `ArduinoCustomFirmware` |

`ArduinoCustomFirmware`: generic host — upload + Command + FrameLog; typed IO when plugin found (`PluginBound`).

Decoder shared: `USensorLabFrameDecoder`.
