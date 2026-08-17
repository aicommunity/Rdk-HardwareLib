# Firmata implementation — tech debt log

## RU

Living document for the Firmata UX implementation plan. Update on every phase gate.

## Open

| ID | Opened | Summary | Reason deferred | Target | Blocker |
|----|--------|---------|-----------------|--------|---------|
| TD-001 | P5 | Manual PWM / Servo / I2C on Uno/Mega | Uno lab still pending | before-release | **Mega 2560 COM3 2026-08-17 passed** (`ArduinoFirmataExtendedProtocols.PwmServoI2cCommandPath`). Uno not run. |

## Hardware integration tests

`Tests/Integration/HardwareLib/Test_ArduinoHardwareIntegration.cpp` — auto-detect USB serial, optional `standard_firmata` upload (avrdude), Firmata handshake, D13 digital, A0 analog, restart.

`Tests/Integration/HardwareLib/Test_ArduinoFirmataExtendedProtocols.cpp` — PWM(D9) / Servo(D10) / I2C smoke on live StandardFirmata. SKIP only if the port cannot be opened (no `ARDUINO_TEST_EXTENDED` flag).

`Tests/Integration/HardwareLib/Test_ArduinoHubLive.cpp` — flash `nmsdk_sensor_hub_v1` / `nmsdk_motor_hub_v1`, PING/`GET PINS` (no sensors required), restore `standard_firmata`.

Windows:

```powershell
.\Scripts\run_arduino_hw_tests.ps1   # default COM3, Mega profile=1
```

```bash
cmake --preset linux-gcc-debug-tests && cmake --build --preset linux-gcc-debug-tests --target Test_ArduinoHardwareIntegration Test_ArduinoFirmataExtendedProtocols Test_ArduinoHubLive
cd Bin/Platform/Linux
sudo usermod -aG dialout $USER && newgrp dialout   # once per machine
ARDUINO_TEST_PORT=/dev/ttyACM0 ARDUINO_BOARD_PROFILE=1 ctest -j1 -R 'ArduinoHardwareIntegration|ArduinoFirmataExtended|ArduinoHubLive' --output-on-failure
```

Env: `ARDUINO_TEST_PORT`, `ARDUINO_BOARD_PROFILE` (0=Uno, 1=Mega), `ARDUINO_SKIP_UPLOAD`, `ARDUINO_FORCE_UPLOAD` (reflash via avrdude), `ARDUINO_TEST_DEBUG`. Test `02` probes Firmata first; upload if probe fails and avrdude is available.

## Closed

| ID | Closed | Summary | Commit / note |
|----|--------|---------|---------------|
| TD-002 | P3 | Pin console digital/PWM via toolbar spins (MVP) | P5 GUI polish |
| TD-003 | P4 | ClDesc + spike 08 | `generate_arduino_cldesc_properties.py`, `08-ArduinoFirmata-AnalogLink` |
| GAP-HUB | P6 | DHT/HC-SR04 / motor timing gaps vs Standard Firmata | Closed by `nmsdk_sensor_hub_v1` + `nmsdk_motor_hub_v1` (HEX, plugins, Hub-Protocols.md, samples 14–15). Firmata DeviceIO remains for pot/servo/digital. |

## Deferred decisions (ADR-lite)

| Date | Decision | Alternatives | Revisit |
|------|----------|--------------|---------|
| 2026-05 | Mega diagram rects from `generate_mega_pins_json.py` grid | Manual SVG-aligned rects | Revisit if click-test fails on hardware |
| 2026-08 | **Keep Mega grid hit-rects** until hardware click-test fails; pinout SVG remains illustrative | Manual SVG-aligned rects (higher effort) | On first Mega pin-miss bug report from GUI |

## Phase 0 note (TD-001)

TD-001 stays **open until Uno lab**: Mega 2560 (`COM3`, `ARDUINO_BOARD_PROFILE=1`) passed PWM(D9)/Servo(D10)/I2C command path on 2026-08-17. Harness no longer requires `ARDUINO_TEST_EXTENDED`; SKIP only if the serial port is missing.

---

## EN

Living document for the Firmata UX implementation plan. Update on every phase gate.

## Open

| ID | Opened | Summary | Reason deferred | Target | Blocker |
|----|--------|---------|-----------------|--------|---------|
| TD-001 | P5 | Manual PWM / Servo / I2C on Uno/Mega | Uno lab still pending | before-release | **Mega 2560 COM3 2026-08-17 passed** (`ArduinoFirmataExtendedProtocols.PwmServoI2cCommandPath`). Uno not run. |

## Hardware integration tests

`Tests/Integration/HardwareLib/Test_ArduinoHardwareIntegration.cpp` — auto-detect USB serial, optional `standard_firmata` upload (avrdude), Firmata handshake, D13 digital, A0 analog, restart.

`Tests/Integration/HardwareLib/Test_ArduinoFirmataExtendedProtocols.cpp` — PWM(D9) / Servo(D10) / I2C smoke on live StandardFirmata. SKIP only if the port cannot be opened (no `ARDUINO_TEST_EXTENDED` flag).

`Tests/Integration/HardwareLib/Test_ArduinoHubLive.cpp` — flash `nmsdk_sensor_hub_v1` / `nmsdk_motor_hub_v1`, PING/`GET PINS` (no sensors required), restore `standard_firmata`.

Windows:

```powershell
.\Scripts\run_arduino_hw_tests.ps1   # default COM3, Mega profile=1
```

```bash
cmake --preset linux-gcc-debug-tests && cmake --build --preset linux-gcc-debug-tests --target Test_ArduinoHardwareIntegration Test_ArduinoFirmataExtendedProtocols Test_ArduinoHubLive
cd Bin/Platform/Linux
sudo usermod -aG dialout $USER && newgrp dialout   # once per machine
ARDUINO_TEST_PORT=/dev/ttyACM0 ARDUINO_BOARD_PROFILE=1 ctest -j1 -R 'ArduinoHardwareIntegration|ArduinoFirmataExtended|ArduinoHubLive' --output-on-failure
```

Env: `ARDUINO_TEST_PORT`, `ARDUINO_BOARD_PROFILE` (0=Uno, 1=Mega), `ARDUINO_SKIP_UPLOAD`, `ARDUINO_FORCE_UPLOAD` (reflash via avrdude), `ARDUINO_TEST_DEBUG`. Test `02` probes Firmata first; upload if probe fails and avrdude is available.

## Closed

| ID | Closed | Summary | Commit / note |
|----|--------|---------|---------------|
| TD-002 | P3 | Pin console digital/PWM via toolbar spins (MVP) | P5 GUI polish |
| TD-003 | P4 | ClDesc + spike 08 | `generate_arduino_cldesc_properties.py`, `08-ArduinoFirmata-AnalogLink` |
| GAP-HUB | P6 | DHT/HC-SR04 / motor timing gaps vs Standard Firmata | Closed by `nmsdk_sensor_hub_v1` + `nmsdk_motor_hub_v1` (HEX, plugins, Hub-Protocols.md, samples 14–15). Firmata DeviceIO remains for pot/servo/digital. |

## Deferred decisions (ADR-lite)

| Date | Decision | Alternatives | Revisit |
|------|----------|--------------|---------|
| 2026-05 | Mega diagram rects from `generate_mega_pins_json.py` grid | Manual SVG-aligned rects | Revisit if click-test fails on hardware |
| 2026-08 | **Keep Mega grid hit-rects** until hardware click-test fails; pinout SVG remains illustrative | Manual SVG-aligned rects (higher effort) | On first Mega pin-miss bug report from GUI |

## Phase 0 note (TD-001)

TD-001 stays **open until Uno lab**: Mega 2560 (`COM3`, `ARDUINO_BOARD_PROFILE=1`) passed PWM(D9)/Servo(D10)/I2C command path on 2026-08-17. Harness no longer requires `ARDUINO_TEST_EXTENDED`; SKIP only if the serial port is missing.
