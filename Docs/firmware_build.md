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
# HardwareLib wrapper (same complete AVR bundle):
./Libraries/Rdk-HardwareLib/Scripts/build_arduino_firmware.sh
```

### Windows (PowerShell)

После [`Scripts/setup_arduino_tools.ps1`](../../../Scripts/setup_arduino_tools.ps1) / [`SetupArduinoTools.bat`](../../../Bin/Platform/Win/SetupArduinoTools.bat) HEX уже собраны. Полная пересборка AVR-набора:

```powershell
.\Scripts\build_arduino_firmware.ps1
```

С явными путями к локальному toolchain:

```powershell
.\Scripts\build_arduino_firmware.ps1 `
  -ArduinoCli "Bin\Platform\Win\ArduinoCLI\arduino-cli.exe" `
  -ArduinoData "Bin\Platform\Win\ArduinoData" `
  -ArduinoUser "Bin\Platform\Win\ArduinoData"
```

См. [Arduino-Setup-Windows.md](Arduino-Setup-Windows.md).

Скрипты собирают StandardFirmata, `sensor_lab`, sensor/motor/I2C/display/pixel/radio/UART hubs для Uno и Mega 2560. Они устанавливают Arduino AVR core и закреплённые версии необходимых Arduino-библиотек, копируют HEX в `Bin/ArduinoFirmware/` и соответствующие каталоги `Firmware/`, затем проверяют каждый HEX-путь из `manifest.json`. `-ArduinoUser` задаёт папку пользовательских библиотек отдельно от Arduino data-каталога; `-SkipLibraries` использует уже установленные библиотеки.

- `Firmware/<sketch>/uno.hex`, `mega2560.hex` для восьми bundled sketches
- `Firmware/firmata/standard_firmata_uno.hex`, `standard_firmata_mega2560.hex`

Bundled AVR HEX не включает ESP32 sketches. I2C default profile оставляет MPU6050 и дополнительные датчики выключенными, чтобы Uno укладывался в flash; список фактически включённых возможностей находится в `Catalog/firmwares/*.json`.

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

After [`Scripts/setup_arduino_tools.ps1`](../../../Scripts/setup_arduino_tools.ps1) / [`SetupArduinoTools.bat`](../../../Bin/Platform/Win/SetupArduinoTools.bat) HEX files are already built. To rebuild separately:

```powershell
.\Scripts\build_arduino_firmware.ps1
```

With explicit paths to the local toolchain:

```powershell
.\Scripts\build_arduino_firmware.ps1 `
  -ArduinoCli "Bin\Platform\Win\ArduinoCLI\arduino-cli.exe" `
  -ArduinoData "Bin\Platform\Win\ArduinoData" `
  -ArduinoUser "Bin\Platform\Win\ArduinoData"
```

See [Arduino-Setup-Windows.md](Arduino-Setup-Windows.md).

The scripts build StandardFirmata, `sensor_lab`, and the sensor/motor/I2C/display/pixel/radio/UART hubs for Uno and Mega 2560. They install the Arduino AVR core and pinned Arduino libraries, copy HEX into `Bin/ArduinoFirmware/` and matching `Firmware/` subdirectories, then validate every HEX path in `manifest.json`. `-ArduinoUser` selects a library folder separately from Arduino data; `-SkipLibraries` uses already installed libraries.

- `Firmware/<sketch>/uno.hex`, `mega2560.hex` for eight bundled sketches
- `Firmware/firmata/standard_firmata_uno.hex`, `standard_firmata_mega2560.hex`

ESP32 sketches are outside this bundled AVR build. The I2C default profile leaves MPU6050 and extra sensors disabled so Uno remains within its flash limit; `Catalog/firmwares/*.json` lists the capabilities actually enabled in bundled HEX.

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
