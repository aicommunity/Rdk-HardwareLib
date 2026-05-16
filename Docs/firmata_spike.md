# Firmata host spike (MVP scope)

## Decision

Implement **minimal in-tree `UArduinoFirmataClient`** (no external firmata-cpp dependency).

## MVP messages (phase 2)

| Message | Status |
|---------|--------|
| SET_PIN_MODE (0xF4) | Implement |
| DIGITAL_MESSAGE (0x90) | Implement |
| ANALOG_MESSAGE (0xE0) | Implement |
| REPORT_ANALOG_PIN (0xC0) | Implement |
| REPORT_DIGITAL_PIN (0xD0) | Implement |
| START_SYSEX / END_SYSEX | Implement |
| FIRMWARE_VERSION (0x79 sysex) | Implement |
| CAPABILITY_QUERY/RESPONSE (0x6B/0x6C) | Implement |
| ANALOG_MAPPING_QUERY/RESPONSE (0x69/0x6A) | Implement |
| I2C_CONFIG/WRITE/READ | Defer phase 3 |
| SERVO_CONFIG | Defer phase 3 |
| STRING_DATA | Defer |

## Baud

Default **57600** (manifest `runtimeBaud`).

## Testing

Uno + Mega: toggle D13 LED via GUI after StandardFirmata flash.
