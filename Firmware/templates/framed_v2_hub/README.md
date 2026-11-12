# Framed v2 hub template

Skeleton sketch for custom / hub firmware using Nmsdk framed protocol v2:

`0xAA | type | len LE | payload | crc8Maxim`

Host side: `ArduinoCustomFirmware` + `IArduinoProtocolPlugin`, or reuse `sensor_lab_v1` plugin.

Build with Arduino CLI / `Scripts/build_arduino_firmware.ps1` after adding a firmware catalog entry.
