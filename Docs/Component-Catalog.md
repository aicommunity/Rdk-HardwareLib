# Rdk-HardwareLib — Component Catalog

Компоненты из [`UHardwareLibrary.cpp`](../Core/UHardwareLibrary.cpp).

## Board / transport / protocol

| ClassName | C++ | Документация |
|-----------|-----|--------------|
| `ArduinoBoard` | `UArduinoBoard` | [Components/ArduinoBoard.md](Components/ArduinoBoard.md) |
| `ArduinoSensorSketch` | `UArduinoSensorSketch` | [Components/ArduinoSensorSketch.md](Components/ArduinoSensorSketch.md) |
| `ArduinoFirmata` | `UArduinoFirmata` | [Components/ArduinoFirmata.md](Components/ArduinoFirmata.md) |

Внутренние (не в палитре Storage): `UArduinoCustomLink`, `UArduinoSerialSession`, `UArduinoFlasher`, `UArduinoBinaryStreamParser` — см. [Transport.md](Transport.md), [Protocol.md](Protocol.md).

## Связанные компоненты

| ClassName | C++ | Связь | Документация |
|-----------|-----|-------|--------------|
| `ArduinoAdc` | `UArduinoAdc` | `LinkedFirmataName` → `ArduinoFirmata` | [Components/ArduinoAdc.md](Components/ArduinoAdc.md) |
| `ArduinoDcDemo` | `UArduinoDcDemo` | `LinkedSketchName` → `ArduinoSensorSketch` | [Components/ArduinoDcDemo.md](Components/ArduinoDcDemo.md) |

## GUI (NeuroModeler)

Target `Rdk-HardwareLib.gui`: diagram + контроллеры Board / SensorSketch / Firmata.

См. [GUI.md](GUI.md). Ресурсы: `GUI/Qt/Resources/boards/*.svg`, `*_pins.json`.

## Firmware

| ID | Описание |
|----|----------|
| `sensor_lab_v1` | Custom sketch DHT/Hall/Servo @ 57600 |
| `standard_firmata` | StandardFirmata Uno/Mega |

- Манифест: [`Firmware/manifest.json`](../Firmware/manifest.json)
- Сборка: [firmware_build.md](firmware_build.md)
- Чеклист: [Firmware/README.md](../Firmware/README.md)

## Миграция

| Старое ClassName | Новое |
|------------------|--------|
| `Arduino` | `ArduinoBoard` + `ArduinoSensorSketch` (или `ArduinoFirmata`) |
| `ADC` | `ArduinoAdc` |
| `DC` | `ArduinoDcDemo` |

Скрипт: `Scripts/migrate_arduino_classnames.py`. Legacy: [Legacy/README.md](Legacy/README.md).

## См. также

- [Architecture.md](Architecture.md)
- [API-Overview.md](API-Overview.md)
- [Usage-Examples.md](Usage-Examples.md)
- [firmata_spike.md](firmata_spike.md)
