# Firmata implementation — tech debt log

Living document for the Firmata UX implementation plan. Update on every phase gate.

## Open

| ID | Opened | Summary | Reason deferred | Target | Blocker |
|----|--------|---------|-----------------|--------|---------|
| TD-001 | P5 | Manual Uno/Mega tests (PWM, Servo, I2C on hardware) | needs-hardware | before-release | — |

## Closed

| ID | Closed | Summary | Commit / note |
|----|--------|---------|---------------|
| TD-002 | P3 | Pin console digital/PWM via toolbar spins (MVP) | P5 GUI polish |
| TD-003 | P4 | ClDesc + spike 08 | `generate_arduino_cldesc_properties.py`, `08-ArduinoFirmata-AnalogLink` |

## Deferred decisions (ADR-lite)

| Date | Decision | Alternatives | Revisit |
|------|----------|--------------|---------|
| 2026-05 | Mega diagram rects from `generate_mega_pins_json.py` grid | Manual SVG-aligned rects | Revisit if click-test fails on hardware |
