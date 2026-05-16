# Rdk-HardwareLib — Component Catalog

Компоненты из `UHardwareLibrary.cpp`, сгруппированы по роли.

## Board / transport

- **ArduinoBoard** (`UArduinoBoard`) — порт, прошивка (bundled HEX / custom path), avrdude upload, heartbeat, auto-reconnect.
- **ArduinoSensorSketch** (`UArduinoSensorSketch`) — кастомный бинарный протокол (sensor_lab), команды, `DoubleMatrixReadings`.
- **ArduinoFirmata** (`UArduinoFirmata`) — Firmata 2.x (in-tree `UArduinoFirmataClient`), pin mode / digital / analog.

## Sensors / demos (phase 3)

- **ArduinoAdc** (`UArduinoAdc`) — чтение аналоговых значений через связанный sketch.
- **ArduinoDcDemo** (`UArduinoDcDemo`) — DC demo: команды и скорость/ускорение из матрицы sketch.

## GUI (NeuroModeler)

Target `Rdk-HardwareLib.gui`: pinout diagram (`UArduinoBoardDiagramWidget`, `UArduinoPinOverlay`), контроллеры Board / SensorSketch / Firmata.

Ресурсы: `GUI/Qt/Resources/boards/*.svg`, `*_pins.json`.

## Firmware

- `Firmware/manifest.json` — `sensor_lab_v1`, `standard_firmata`.
- Исходник: `Firmware/sensor_lab/sensor_lab.ino` @ 57600.
- Сборка HEX: `Scripts/build_arduino_firmware.sh` (требует `arduino-cli` и доступ к downloads.arduino.cc).

## Миграция со старых имён

| Старое ClassName | Новое |
|------------------|--------|
| Arduino | ArduinoBoard + ArduinoSensorSketch (или ArduinoFirmata) |
| ADC | ArduinoAdc |
| DC | ArduinoDcDemo |

Скрипт: `Scripts/migrate_arduino_classnames.py`.

## См. также

- [Architecture.md](Architecture.md)
- [firmware_build.md](firmware_build.md)
- [firmata_spike.md](firmata_spike.md)
- Компонентные заметки в `Docs/Components/` (исторические диаграммы для `UArduinoControl` — см. заголовки файлов).
