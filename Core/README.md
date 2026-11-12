# Core layout (Rdk-HardwareLib)

```
Core/
  UHardwareLibrary.*     # library registration only
  README.md
  Board/                 # Board, Firmata, Manifest, PropertyString, SampleBuffer
  Links/                 # CustomLink, SensorSketch, DcDemo, Adc, CustomFirmware
  Catalog/               # UHardwareCatalog, UHardwareSetup
  Assembly/              # UArduinoAssemblySceneBuilder
  Devices/               # DeviceIO, PinResolver, BatchBuilder
  Protocol/              # FirmataClient, BinaryParser, Plugins
  Transport/             # Serial, Flasher, PinMap, UploadJob
  Legacy/                # not linked (if present)
```

CMake PUBLIC includes: `Core` + each subdirectory above (except Legacy).

See [ADR-Core-Layout.md](../Docs/ADR-Core-Layout.md).
