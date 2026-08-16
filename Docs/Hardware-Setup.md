# Hardware Setup

## RU

JSON-описание сборки платы: `board`, `firmwareId`, `stack` (щиты), `devices[]`.

Валидация: `UHardwareSetup` + `UHardwareCatalog` (корень: `Bin/HardwareCatalog` или `RDK_HARDWARE_CATALOG_DIR` / `NMSDK_SOURCE_DIR/.../Catalog`).

Свойства `ArduinoBoard`: `HardwareSetupPath`, `HardwareSetupJson`, `HardwareSetupValid`, `HardwareSetupIssues`.

Коды issues: `PinConflict`, `FirmwareShieldIncompatible`, `ModuleCapabilityMissing`, `UnknownId`, `ReservedPort`, `TimingPreferHub`, `FirmwareUnavailable`, `BoardMismatch`, `SchemaVersion`.

Синхронизация: `Scripts/sync_hardware_catalog.py`.

## EN

See RU. Catalog sync via `Scripts/sync_hardware_catalog.py`.
