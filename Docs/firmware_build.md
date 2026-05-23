# Firmware build

Сборка bundled HEX в `Bin/ArduinoFirmware/` (runtime). Исходники sketch: `Libraries/Rdk-HardwareLib/Firmware/`.

## Требования

- [arduino-cli](https://arduino.github.io/arduino-cli/)
- Сеть для `arduino-cli core install` (при 403 на downloads.arduino.cc — соберите на другой машине и закоммитьте `.hex`)

## Сборка из корня Nmsdk

```bash
./Scripts/build_arduino_firmware.sh
```

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
