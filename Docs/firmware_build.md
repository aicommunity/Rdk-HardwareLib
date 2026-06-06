# Firmware build

## RU

Сборка bundled HEX в `Bin/ArduinoFirmware/` (runtime). Исходники sketch: `Libraries/Rdk-HardwareLib/Firmware/`.

## Требования

- [arduino-cli](https://arduino.github.io/arduino-cli/)
- Сеть для `arduino-cli core install` (при 403 на downloads.arduino.cc — соберите на другой машине и закоммитьте `.hex`)

## Сборка из корня Nmsdk

### Linux / macOS

```bash
./Scripts/build_arduino_firmware.sh
```

### Windows (PowerShell)

После [`SetupArduinoTools.bat`](../../../Bin/Platform/Win/SetupArduinoTools.bat) HEX уже собраны. Отдельная пересборка:

```powershell
.\Scripts\build_arduino_firmware.ps1
```

С явными путями к локальному toolchain:

```powershell
.\Scripts\build_arduino_firmware.ps1 `
  -ArduinoCli "Bin\Platform\Win\ArduinoCLI\arduino-cli.exe" `
  -ArduinoData "Bin\Platform\Win\ArduinoData"
```

См. [Arduino-Setup-Windows.md](Arduino-Setup-Windows.md).

Скрипт устанавливает ядро `arduino:avr`, библиотеку DHT, компилирует `sensor_lab` (Uno + Mega) и StandardFirmata, копирует артефакты в:

- `Firmware/sensor_lab/uno.hex`, `mega2560.hex`
- `Firmware/firmata/standard_firmata_uno.hex`, `standard_firmata_mega2560.hex`

## Ручная сборка sensor_lab

```bash
arduino-cli core install arduino:avr
arduino-cli lib install "DHT sensor library"
arduino-cli compile --fqbn arduino:avr:uno \
  Libraries/Rdk-HardwareLib/Firmware/sensor_lab
cp build/sensor_lab.ino.hex Libraries/Rdk-HardwareLib/Firmware/sensor_lab/uno.hex

arduino-cli compile --fqbn arduino:avr:mega \
  Libraries/Rdk-HardwareLib/Firmware/sensor_lab
cp build/sensor_lab.ino.hex Libraries/Rdk-HardwareLib/Firmware/sensor_lab/mega2560.hex
```

## StandardFirmata

Через IDE (пример File → Examples → Firmata → StandardFirmata) или `arduino-cli compile` с соответствующим sketch.

## После сборки

- Обновите `Bin/ArduinoFirmware/manifest.json` (и шаблон в `Libraries/.../Firmware/`) при смене путей или id.
- Проверка на хосте: unit-тест `ArduinoFirmwareManifest.AllBundledIdsResolveHex`.
- Ручная проверка на плате: [Firmware/README.md](../Firmware/README.md).

---

## EN

Build bundled HEX into `Bin/ArduinoFirmware/` (runtime). Sketch sources: `Libraries/Rdk-HardwareLib/Firmware/`.

## Requirements

- [arduino-cli](https://arduino.github.io/arduino-cli/)
- Network for `arduino-cli core install` (if 403 on downloads.arduino.cc — build on another machine and commit the `.hex`)

## Build from Nmsdk root

### Linux / macOS

```bash
./Scripts/build_arduino_firmware.sh
```

### Windows (PowerShell)

After [`SetupArduinoTools.bat`](../../../Bin/Platform/Win/SetupArduinoTools.bat) HEX files are already built. To rebuild separately:

```powershell
.\Scripts\build_arduino_firmware.ps1
```

With explicit paths to the local toolchain:

```powershell
.\Scripts\build_arduino_firmware.ps1 `
  -ArduinoCli "Bin\Platform\Win\ArduinoCLI\arduino-cli.exe" `
  -ArduinoData "Bin\Platform\Win\ArduinoData"
```

See [Arduino-Setup-Windows.md](Arduino-Setup-Windows.md).

The script installs the `arduino:avr` core, the DHT library, compiles `sensor_lab` (Uno + Mega) and StandardFirmata, and copies artifacts to:

- `Firmware/sensor_lab/uno.hex`, `mega2560.hex`
- `Firmware/firmata/standard_firmata_uno.hex`, `standard_firmata_mega2560.hex`

## Manual sensor_lab build

```bash
arduino-cli core install arduino:avr
arduino-cli lib install "DHT sensor library"
arduino-cli compile --fqbn arduino:avr:uno \
  Libraries/Rdk-HardwareLib/Firmware/sensor_lab
cp build/sensor_lab.ino.hex Libraries/Rdk-HardwareLib/Firmware/sensor_lab/uno.hex

arduino-cli compile --fqbn arduino:avr:mega \
  Libraries/Rdk-HardwareLib/Firmware/sensor_lab
cp build/sensor_lab.ino.hex Libraries/Rdk-HardwareLib/Firmware/sensor_lab/mega2560.hex
```

## StandardFirmata

Via IDE (example File → Examples → Firmata → StandardFirmata) or `arduino-cli compile` with the corresponding sketch.

## After build

- Update `Bin/ArduinoFirmware/manifest.json` (and the template in `Libraries/.../Firmware/`) when paths or ids change.
- Host verification: unit test `ArduinoFirmwareManifest.AllBundledIdsResolveHex`.
- Manual board verification: [Firmware/README.md](../Firmware/README.md).
