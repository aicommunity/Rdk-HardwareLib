# ArduinoDcDemo

## RU

## Назначение

**ClassName:** `ArduinoDcDemo`  
**C++:** `UArduinoDcDemo` : `UArduinoCustomLink` : `UArduinoBoard`  
**Роль:** Демо DC-мотора — serial, прошивка `sensor_lab_v1`, команды и чтение скорости в **одном узле**.

## Ключевые свойства

| Свойство | Роль |
|----------|------|
| `PortName`, `BundledFirmwareId` | Порт и прошивка (как у Board/Sketch) |
| `Connect` / `Disconnect` | Edge подключения (вкладка Board в GUI) |
| `Command` | Строка команды |
| `SendCommand` | Edge: отправить `Command` |
| `GetSpeed` | Edge: запросить скорость |
| `Speed` / `Acceleration` | State (из binary frame `0x01` в `OnBinaryFrame`) |

## Deprecated

`LinkedSketchName` — если не пуст, делегирует команды в `ArduinoSensorSketch` (переходный релиз). **Новые схемы:** один `ArduinoDcDemo` без sketch.

## Типичная схема

```mermaid
flowchart LR
  Dc[ArduinoDcDemo]
```

Прошивка: `sensor_lab_v1`, `BaudRate` 57600.

## GUI

`hw.arduino.dc_demo` — вкладки **DC** (команды, presets) и **Board** (порт, Connect, upload).

## Тестовый конфиг

`Bin/Configs/SpikeSamples/Hardware/05-ArduinoDcDemo/`

## ClDesc

`Bin/ClDesc/HardwareLibrary/ru-RU/ArduinoDcDemo.xml`

## Миграция

Два узла `SensorSketch` + `DcDemo` → один `DcDemo`: `Scripts/migrate_arduino_board_hierarchy.py` или regen `Scripts/generate_arduino_hardware_configs.py`.

---

## EN

## Purpose

**ClassName:** `ArduinoDcDemo`  
**C++:** `UArduinoDcDemo` : `UArduinoCustomLink` : `UArduinoBoard`  
**Role:** DC motor demo — serial, `sensor_lab_v1` firmware, commands and speed reading in **one node**.

## Key properties

| Property | Role |
|----------|------|
| `PortName`, `BundledFirmwareId` | Port and firmware (as in Board/Sketch) |
| `Connect` / `Disconnect` | Connection edges (Board tab in GUI) |
| `Command` | Command string |
| `SendCommand` | Edge: send `Command` |
| `GetSpeed` | Edge: request speed |
| `Speed` / `Acceleration` | State (from binary frame `0x01` in `OnBinaryFrame`) |

## Deprecated

`LinkedSketchName` — if non-empty, delegates commands to `ArduinoSensorSketch` (transitional release). **New schematics:** single `ArduinoDcDemo` without sketch.

## Typical layout

```mermaid
flowchart LR
  Dc[ArduinoDcDemo]
```

Firmware: `sensor_lab_v1`, `BaudRate` 57600.

## GUI

`hw.arduino.dc_demo` — **DC** (commands, presets) and **Board** (port, Connect, upload) tabs.

## Test config

`Bin/Configs/SpikeSamples/Hardware/05-ArduinoDcDemo/`

## ClDesc

`Bin/ClDesc/HardwareLibrary/ru-RU/ArduinoDcDemo.xml`

## Migration

Two nodes `SensorSketch` + `DcDemo` → one `DcDemo`: `Scripts/migrate_arduino_board_hierarchy.py` or regen `Scripts/generate_arduino_hardware_configs.py`.
