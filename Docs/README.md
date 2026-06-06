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
- [Arduino-Setup-Windows.md](Arduino-Setup-Windows.md) — SetupArduinoTools.bat, bundled ArduinoTools
- [Protocol.md](Protocol.md) — бинарный протокол sensor_lab
- [GUI.md](GUI.md) — NeuroModeler forms и diagram
- [firmata_spike.md](firmata_spike.md) — scope Firmata MVP
- [FirmataTechDebt.md](FirmataTechDebt.md) — tech debt log (Firmata implementation)

### Doxygen

Markdown в `Docs/` — основное руководство. API из заголовков `Core/` — см. [DOXYGEN.md](DOXYGEN.md).

### Корневая документация Nmsdk

- [Docs/Libraries/Rdk-HardwareLib.md](../../../Docs/Libraries/Rdk-HardwareLib.md)

---

## EN

### Purpose

**Rdk-HardwareLib** provides NeuroModeler components for Arduino boards (Uno / Mega 2560) over USB serial: HEX firmware upload, custom `sensor_lab` sketch, Standard Firmata, GUI pinout.

### Components (ClassName)

| ClassName | Purpose |
|-----------|---------|
| `ArduinoBoard` | Port, avrdude upload, heartbeat, reconnect |
| `ArduinoSensorSketch` | sensor_lab protocol, commands, readings matrix |
| `ArduinoFirmata` | Firmata: pin mode / digital / analog |
| `ArduinoAdc` | ADC reading via linked `ArduinoFirmata` |
| `ArduinoDcDemo` | DC demo (single node, `UArduinoCustomLink` + sensor_lab_v1) |

Full catalog: [Component-Catalog.md](Component-Catalog.md).

### C++ naming conventions

- **Class members** (fields, including private): `CamelCase` (`Session`, `ProtocolVersionValue`, `BoardPanel`).
- **Local variables and method parameters**: `snake_case` (`port_name`, `param_count`, `board_profile`).
- **`UProperty` names** (XML/schema): unchanged (`PortName`, `Connect`, …).

### Property-driven control

Connection and actions use **edge properties** (`Connect`, `SendCommand`, `UploadFirmware`, …): pulse `true` for one calculation tick, then reset to `false`. GUI: `HardwareGuiHelpers::pulseEdge`. See [Architecture.md](Architecture.md), [API-Overview.md](API-Overview.md).

### Quick start

1. Connect the board and verify the port (`/dev/ttyACM0` on Linux).
2. Add your user to the `dialout` group (Linux): `sudo usermod -aG dialout $USER`, then re-login.
3. In the model: `ArduinoBoard` component → `PortName`, `BaudRate` **57600**, `BundledFirmwareId` = `sensor_lab_v1`.
4. In the GUI: **Upload firmware**, then `ArduinoSensorSketch` with `ConnectOnBuild` = true.

Sample configs: [Bin/Configs/SpikeSamples/Hardware/README.md](../../../Bin/Configs/SpikeSamples/Hardware/README.md).

### Baud rate and firmware

- Runtime baud: **57600** (sensor_lab and bundled Firmata).
- Bundled HEX: `Firmware/manifest.json` — `sensor_lab_v1`, `standard_firmata`.
- Build: [firmware_build.md](firmware_build.md), hardware checklist: [Firmware/README.md](../Firmware/README.md).

### Migration from legacy names

| Old | New |
|-----|-----|
| `Arduino` | `ArduinoBoard` + `ArduinoSensorSketch` (or `ArduinoFirmata`) |
| `ADC` | `ArduinoAdc` |
| `DC` | `ArduinoDcDemo` |

Script: `Scripts/migrate_arduino_classnames.py`. Historical docs: [Legacy/README.md](Legacy/README.md).

### Navigation

- [Architecture.md](Architecture.md) — class hierarchy and runtime
- [API-Overview.md](API-Overview.md) — component properties
- [Usage-Examples.md](Usage-Examples.md) — XML and scenarios
- [Transport.md](Transport.md) — serial, avrdude, board profiles
- [Arduino-Setup-Windows.md](Arduino-Setup-Windows.md) — SetupArduinoTools.bat, bundled ArduinoTools
- [Protocol.md](Protocol.md) — sensor_lab binary protocol
- [GUI.md](GUI.md) — NeuroModeler forms and diagram
- [firmata_spike.md](firmata_spike.md) — Firmata MVP scope
- [FirmataTechDebt.md](FirmataTechDebt.md) — tech debt log (Firmata implementation)

### Doxygen

Markdown in `Docs/` is the primary guide. API from `Core/` headers — see [DOXYGEN.md](DOXYGEN.md).

### Nmsdk root documentation

- [Docs/Libraries/Rdk-HardwareLib.md](../../../Docs/Libraries/Rdk-HardwareLib.md)

---
