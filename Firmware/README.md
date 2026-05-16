# Bundled firmware

| Path | Description |
|------|-------------|
| `sensor_lab/sensor_lab.ino` | Custom protocol sketch (57600 baud) |
| `sensor_lab/uno.hex`, `mega2560.hex` | Prebuilt sensor_lab (run build script) |
| `firmata/standard_firmata_*.hex` | StandardFirmata for Uno / Mega |
| `manifest.json` | IDs resolved by `UFirmwareManifest` |

Build all HEX files:

```bash
./Scripts/build_arduino_firmware.sh
```

If `downloads.arduino.cc` returns 403, build on a machine with working Arduino CLI and commit the `.hex` files.
