# QA: Wheeled robots & modules catalog

## L0–L4 (CI)

- Build `Rdk-HardwareLib.qt` / `.gui`
- `Test_ArduinoBoardEdges`, `Test_ArduinoComponentGuiRegistry`
- `Test_WheeledRobotEdges`, `Test_WaveshareUgvJson`, `Test_HardwareModulesCatalog` (when present)

## L5 Integration (optional hardware)

Env: `WHEELED_PORT=/dev/ttyACM0`, `ARDUINO_SKIP_UPLOAD=1` for connect-only.
Without port: tests **SKIP** (must not FAIL).

## L6 Manual checklist

### Without board

- [ ] Palette: Esp32Board, ArduinoWheeledRobot, Esp32WheeledRobot, WaveRover
- [ ] Samples 09–12 open without crash; Drive/Board/Assembly tabs
- [ ] Drive Apply/Stop pulses edges
- [ ] Catalog lists ≥70 modules after sync

### Motor Shield R3 + Uno

- [ ] Upload `nmsdk_motor_hub_v1`
- [ ] ApplyDrive independent A/B; Stop; USB unplug → watchdog stop

### WaveRover V0.9

- [ ] Connect 115200; ApplyDrive; retransmit keeps motion; Stop → zeros

### ESP32 + L298N

- [ ] Esp32WheeledRobot Connect; ApplyDrive; Stop
