# ADR: Core directory layout (HardwareLib)

## Status

Accepted (phase 5 consolidation).

## Context

Arduino HardwareLib grew flat under `Core/` (Board, Firmata, CustomLink, SensorSketch, Catalog, Devices, plugins). Include and review cost rose.

## Decision

```
Core/
  UHardwareLibrary.*     # library entry only
  README.md
  Board/                 # UArduinoBoard, Firmata, Manifest, PropertyString, SampleBuffer
  Links/                 # CustomLink, SensorSketch, DcDemo, Adc, CustomFirmware
  Catalog/ Assembly/ Devices/ Protocol/ Transport/
  Legacy/                # excluded from CMake (if present)
```

Public include dirs list each subfolder so existing `#include "UArduinoBoard.h"` keeps working.

## Consequences

- Tests/GUI that used `Core/UFoo.h` paths must use `Core/Board/` or `Core/Links/`.
- New code should land in the matching subdirectory.
- Legacy Connect/Control classes are not built.
