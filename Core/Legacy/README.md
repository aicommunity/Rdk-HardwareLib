# Legacy Core (excluded from build)

Orphan Arduino classes (`UArduinoConnect`, `UArduinoControl`, `UAdcSensor`, `UDcControlDemo`) were removed from the live tree; replacements live under `Core/Board`, `Core/Links`, and `Core/Transport`.

This folder is intentionally empty of sources and is **not** listed in `CMakeLists.txt` GLOB/REQUIRED sources. Keep any future deprecated sources here only if a temporary compile escape hatch is required.
