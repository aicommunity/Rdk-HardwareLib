# Modules Catalog

## RU

Каталог модулей Arduino/ESP32 импортирован из
[`Docs/ArduinoShields/arduino-esp32-modules-catalog.xlsx`](ArduinoShields/arduino-esp32-modules-catalog.xlsx)
скриптом [`Scripts/import_modules_catalog_xlsx.py`](../Scripts/import_modules_catalog_xlsx.py).

JSON: `Catalog/modules/<id>.json`. Индекс: `Catalog/catalog.json`.
Зеркало runtime: `Bin/HardwareCatalog` (`Scripts/sync_hardware_catalog.py`).

Поле `runtime`: `firmata` | `hub` | `motor_hub` | `planned`.

Перегенерация:

```bash
cd Libraries/Rdk-HardwareLib/Scripts
python3 import_modules_catalog_xlsx.py
python3 sync_hardware_catalog.py --dest /path/to/Bin/HardwareCatalog
```

## EN

Module JSON catalog generated from the xlsx sheet «Каталог». See RU section for paths and runtime badges.
