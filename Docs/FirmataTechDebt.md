# Firmata implementation — tech debt log

## RU

Living document for the Firmata UX implementation plan. Update on every phase gate.

## Open

| ID | Opened | Summary | Reason deferred | Target | Blocker |
|----|--------|---------|-----------------|--------|---------|
| TD-001 | P5 | Manual PWM / Servo / I2C on Uno/Mega (not in integration suite) | needs-hardware | before-release | Physical board; see `Test_ArduinoFirmataExtendedProtocols` (skips without hardware) |

## Hardware integration tests

`Tests/Integration/HardwareLib/Test_ArduinoHardwareIntegration.cpp` — auto-detect USB serial, optional `standard_firmata` upload (avrdude), Firmata handshake, D13 digital, A0 analog, restart.

`Tests/Integration/HardwareLib/Test_ArduinoFirmataExtendedProtocols.cpp` — PWM / Servo / I2C smoke when `ARDUINO_TEST_EXTENDED=1` and board present; otherwise `GTEST_SKIP`.

```bash
cmake --preset linux-gcc-debug-tests && cmake --build --preset linux-gcc-debug-tests --target Test_ArduinoHardwareIntegration Test_ArduinoFirmataExtendedProtocols
cd Bin/Platform/Linux
sudo usermod -aG dialout $USER && newgrp dialout   # once per machine
ctest -R 'ArduinoHardwareIntegration|ArduinoFirmataExtended' --output-on-failure
# Extended (optional): ARDUINO_TEST_EXTENDED=1 ARDUINO_TEST_PORT=/dev/ttyUSB0 ctest -R ArduinoFirmataExtended
```

Env: `ARDUINO_TEST_PORT`, `ARDUINO_BOARD_PROFILE` (0=Uno, 1=Mega), `ARDUINO_SKIP_UPLOAD`, `ARDUINO_FORCE_UPLOAD` (reflash via avrdude), `ARDUINO_TEST_DEBUG`, `ARDUINO_TEST_EXTENDED`. Test `02` probes Firmata first; upload only if probe fails and `ARDUINO_FORCE_UPLOAD=1`.

## Closed

| ID | Closed | Summary | Commit / note |
|----|--------|---------|---------------|
| TD-002 | P3 | Pin console digital/PWM via toolbar spins (MVP) | P5 GUI polish |
| TD-003 | P4 | ClDesc + spike 08 | `generate_arduino_cldesc_properties.py`, `08-ArduinoFirmata-AnalogLink` |

## Deferred decisions (ADR-lite)

| Date | Decision | Alternatives | Revisit |
|------|----------|--------------|---------|
| 2026-05 | Mega diagram rects from `generate_mega_pins_json.py` grid | Manual SVG-aligned rects | Revisit if click-test fails on hardware |

---

## EN

Living document for the Firmata UX implementation plan. Update on every phase gate.

## Open

| ID | Opened | Summary | Reason deferred | Target | Blocker |
|----|--------|---------|-----------------|--------|---------|
| TD-001 | P5 | Manual PWM / Servo / I2C on Uno/Mega (not in integration suite) | needs-hardware | before-release | Physical board; see `Test_ArduinoFirmataExtendedProtocols` (skips without hardware) |

## Hardware integration tests

`Tests/Integration/HardwareLib/Test_ArduinoHardwareIntegration.cpp` — auto-detect USB serial, optional `standard_firmata` upload (avrdude), Firmata handshake, D13 digital, A0 analog, restart.

`Tests/Integration/HardwareLib/Test_ArduinoFirmataExtendedProtocols.cpp` — PWM / Servo / I2C smoke when `ARDUINO_TEST_EXTENDED=1` and board present; otherwise `GTEST_SKIP`.

```bash
cmake --preset linux-gcc-debug-tests && cmake --build --preset linux-gcc-debug-tests --target Test_ArduinoHardwareIntegration Test_ArduinoFirmataExtendedProtocols
cd Bin/Platform/Linux
sudo usermod -aG dialout $USER && newgrp dialout   # once per machine
ctest -R 'ArduinoHardwareIntegration|ArduinoFirmataExtended' --output-on-failure
# Extended (optional): ARDUINO_TEST_EXTENDED=1 ARDUINO_TEST_PORT=/dev/ttyUSB0 ctest -R ArduinoFirmataExtended
```

Env: `ARDUINO_TEST_PORT`, `ARDUINO_BOARD_PROFILE` (0=Uno, 1=Mega), `ARDUINO_SKIP_UPLOAD`, `ARDUINO_FORCE_UPLOAD` (reflash via avrdude), `ARDUINO_TEST_DEBUG`, `ARDUINO_TEST_EXTENDED`. Test `02` probes Firmata first; upload only if probe fails and `ARDUINO_FORCE_UPLOAD=1`.

## Closed

| ID | Closed | Summary | Commit / note |
|----|--------|---------|---------------|
| TD-002 | P3 | Pin console digital/PWM via toolbar spins (MVP) | P5 GUI polish |
| TD-003 | P4 | ClDesc + spike 08 | `generate_arduino_cldesc_properties.py`, `08-ArduinoFirmata-AnalogLink` |

## Deferred decisions (ADR-lite)

| Date | Decision | Alternatives | Revisit |
|------|----------|--------------|---------|
| 2026-05 | Mega diagram rects from `generate_mega_pins_json.py` grid | Manual SVG-aligned rects | Revisit if click-test fails on hardware |
