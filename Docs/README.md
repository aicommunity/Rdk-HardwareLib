# Rdk-HardwareLib — документация

## RU

### Назначение

**Rdk-HardwareLib** — компоненты NeuroModeler для работы с Arduino (Uno / Mega 2560) по USB serial: прошивка HEX, custom sketch `sensor_lab`, Standard Firmata, GUI pinout.

### Компоненты (ClassName)

| ClassName | Назначение |
|-----------|------------|
| `ArduinoBoard` | Порт, avrdude upload, heartbeat, reconnect |
| `ArduinoSensorSketch` | Протокол sensor_lab, команды, матрица показаний |
| `ArduinoFirmata` | Firmata: pin mode / digital / analog |
| `ArduinoAdc` | Чтение ADC через связанный `ArduinoFirmata` |
| `ArduinoDcDemo` | DC demo (один узел, `UArduinoCustomLink` + sensor_lab_v1) |

Полный каталог: [Component-Catalog.md](Component-Catalog.md).

### Соглашения об именовании (C++)

- **Члены класса** (поля, в т.ч. private): `CamelCase` (`Session`, `ProtocolVersionValue`, `BoardPanel`).
- **Локальные переменные и параметры методов**: `snake_case` (`port_name`, `param_count`, `board_profile`).
- **Свойства `UProperty`** (имена в XML/схеме): без изменений (`PortName`, `Connect`, …).

### Property-driven control

Подключение и действия — через **edge-свойства** (`Connect`, `SendCommand`, `UploadFirmware`, …): импульс `true` на один тик расчёта, затем сброс в `false`. GUI: `HardwareGuiHelpers::pulseEdge`. Подробнее: [Architecture.md](Architecture.md), [API-Overview.md](API-Overview.md).

### Быстрый старт

1. Подключите плату, проверьте порт (`/dev/ttyACM0` на Linux).
2. Добавьте пользователя в группу `dialout` (Linux): `sudo usermod -aG dialout $USER`, перелогиньтесь.
3. В модели: компонент `ArduinoBoard` → `PortName`, `BaudRate` **57600**, `BundledFirmwareId` = `sensor_lab_v1`.
4. В GUI: **Upload firmware**, затем `ArduinoSensorSketch` с `ConnectOnBuild` = true.

Тестовые конфиги: [Bin/Configs/SpikeSamples/Hardware/README.md](../../../Bin/Configs/SpikeSamples/Hardware/README.md).

### Скорость и прошивки

- Runtime baud: **57600** (sensor_lab и bundled Firmata).
- Bundled HEX: `Firmware/manifest.json` — `sensor_lab_v1`, `standard_firmata`.
- Сборка: [firmware_build.md](firmware_build.md), чеклист на железе: [Firmware/README.md](../Firmware/README.md).

### Миграция со старых имён

| Было | Стало |
|------|--------|
| `Arduino` | `ArduinoBoard` + `ArduinoSensorSketch` (или `ArduinoFirmata`) |
| `ADC` | `ArduinoAdc` |
| `DC` | `ArduinoDcDemo` |

Скрипт: `Scripts/migrate_arduino_classnames.py`. Исторические документы: [Legacy/README.md](Legacy/README.md).

### Навигация

- [Architecture.md](Architecture.md) — иерархия классов и runtime
- [API-Overview.md](API-Overview.md) — свойства компонентов
- [Usage-Examples.md](Usage-Examples.md) — XML и сценарии
- [Transport.md](Transport.md) — serial, avrdude, профили плат
- [Protocol.md](Protocol.md) — бинарный протокол sensor_lab
- [GUI.md](GUI.md) — NeuroModeler forms и diagram
- [firmata_spike.md](firmata_spike.md) — scope Firmata MVP

### Doxygen

Markdown в `Docs/` — основное руководство. API из заголовков `Core/` — см. [DOXYGEN.md](DOXYGEN.md).

### Корневая документация Nmsdk

- [Docs/Libraries/Rdk-HardwareLib.md](../../../Docs/Libraries/Rdk-HardwareLib.md)

---

## EN

### Purpose

**Rdk-HardwareLib** provides NeuroModeler components for Arduino boards over USB serial (firmware upload, sensor_lab, Firmata, GUI pinout).

### Components

See [Component-Catalog.md](Component-Catalog.md) for `ArduinoBoard`, `ArduinoSensorSketch`, `ArduinoFirmata`, `ArduinoAdc`, `ArduinoDcDemo`.

### Quick start

Set `PortName`, `BaudRate` **57600**, flash via `ArduinoBoard`, then use `ArduinoSensorSketch` or `ArduinoFirmata`. Sample configs: `Bin/Configs/SpikeSamples/Hardware/`.

### Detailed docs

[Architecture.md](Architecture.md), [API-Overview.md](API-Overview.md), [Usage-Examples.md](Usage-Examples.md).
