# QA: Wheeled robots & modules catalog

## L0–L4 (CI)

- Build `Rdk-HardwareLib.qt` / `.gui` on Linux and Windows (MSVC)
- `Test_ArduinoBoardEdges`, `Test_ArduinoComponentGuiRegistry`
- `Test_WheeledRobotEdges`, `Test_WaveshareUgvJson`, `Test_HardwareModulesCatalog`
- `Test_MotorDriverSetPin`, `Test_NmsdkMotorHubPluginDual`, `Test_SensorHubFrameDecode`, `Test_ArduinoSampleBuffer`
- Windows-focused regression set: `NmeaGpsParser|NmsdkMotorHubPluginDual|MotorDriverSetPinL2|DeviceIOModulePins|HardwareModulesCatalog|ArduinoSampleBuffer|HubPlugins`
- AVR firmware matrix: StandardFirmata and sensor/motor/I2C/display/pixel/radio/UART sketches for Uno and Mega 2560

## L5 Integration (optional hardware)

Env: `WHEELED_PORT=/dev/ttyACM0`, `ARDUINO_SKIP_UPLOAD=1` for connect-only.
Without port: tests **SKIP** (must not FAIL).

Windows serial integration should use the assigned `COMn` device and the same no-upload option. USB unplug/watchdog behavior must be checked on a physical board; host tests cannot establish electrical safety behavior.

## L6 Manual checklist

### Without board

- [ ] Palette: Esp32Board, ArduinoWheeledRobot, Esp32WheeledRobot, WaveRover
- [ ] Samples 09–12 open without crash; Drive/Board/Assembly tabs
- [ ] Drive Apply/Stop pulses edges
- [ ] Catalog lists ≥70 modules after sync

### Motor Shield R3 + Uno

- [ ] Upload `nmsdk_motor_hub_v1`
- [ ] ApplyDrive independent A/B; Stop; USB unplug → watchdog stop
- [ ] Verify L298N `dir2_pwm` channel mapping and direction on a supported board

### WaveRover V0.9

- [ ] Connect 115200; ApplyDrive; retransmit keeps motion; Stop → zeros

### ESP32 + L298N

- [ ] Esp32WheeledRobot Connect; ApplyDrive; Stop
- [ ] Compile the ESP32 firmware variant with the selected ESP32 Arduino core before claiming ESP32 firmware support

## Current audit evidence (2026-10-08)

- Windows MSVC 2019 selected host regression tests: 15/15 passed.
- AVR Uno/Mega firmware matrix compiled and manifest paths validated.
- ESP32 firmware compilation and all physical-board checklist items remain pending.
