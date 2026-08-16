# Core layout (Rdk-HardwareLib)

```
Core/
  Board/          # planned: UArduinoBoard (+ property helpers) — currently still in Core/
  Catalog/        # UHardwareCatalog, UHardwareSetup
  Assembly/       # UArduinoAssemblySceneBuilder
  Devices/        # UArduinoDeviceIO, PinResolver, BatchBuilder
  Links/          # planned: CustomLink / SensorSketch / DcDemo / CustomFirmware
  Protocol/       # FirmataClient, BinaryParser, Plugins
  Transport/      # Serial, Flasher, PinMap, UploadJob
  *.cpp           # Board / Firmata / SensorSketch / DcDemo / CustomFirmware (transitional)
```

Phase 5 consolidates includes via CMake `Core/*` GLOBs. Prefer new code under Catalog/Devices/Protocol/Assembly.
