# Firmata host (implemented scope)

## RU

## Decision

In-tree **`UArduinoFirmataClient`** + **`UArduinoFirmata`** component (no firmata-cpp).

## Implemented protocol

| Message | Status |
|---------|--------|
| SET_PIN_MODE (0xF4) | Done |
| SET_DIGITAL_PIN_VALUE (0xF5) | Done |
| DIGITAL_MESSAGE (0x90) RX | Done |
| ANALOG_MESSAGE (0xE0) RX | Done |
| REPORT_ANALOG (0xC0) by **channel** | Done |
| REPORT_DIGITAL (0xD0) | Done |
| SAMPLING_INTERVAL (0x7A) | Done |
| FIRMWARE / CAPABILITY / ANALOG_MAPPING | Done |
| EXTENDED_ANALOG (0x6F) PWM | Done |
| PIN_STATE_QUERY (0x6D/0x6E) | Done |
| SERVO_CONFIG / servo write | Done (MVP) |
| I2C config/write/read | Done (MVP) |

## GUI

- Pin console table (Uno/Mega full range)
- Monitor tab: `AnalogSamples` preview, `StreamLog`
- I2C tab
- Diagram overlay from `PinStatusJson`

## Baud

Default **57600** (`runtimeBaud` in manifest).

## Testing

- Unit: `ctest -R ArduinoFirmataClient`, `ArduinoPinMap`, `ArduinoSampleBuffer`
- Hardware: Uno/Mega D13, A0, PWM D9 — see [FirmataTechDebt.md](FirmataTechDebt.md) TD-001

---

## EN

## Decision

In-tree **`UArduinoFirmataClient`** + **`UArduinoFirmata`** component (no firmata-cpp).

## Implemented protocol

| Message | Status |
|---------|--------|
| SET_PIN_MODE (0xF4) | Done |
| SET_DIGITAL_PIN_VALUE (0xF5) | Done |
| DIGITAL_MESSAGE (0x90) RX | Done |
| ANALOG_MESSAGE (0xE0) RX | Done |
| REPORT_ANALOG (0xC0) by **channel** | Done |
| REPORT_DIGITAL (0xD0) | Done |
| SAMPLING_INTERVAL (0x7A) | Done |
| FIRMWARE / CAPABILITY / ANALOG_MAPPING | Done |
| EXTENDED_ANALOG (0x6F) PWM | Done |
| PIN_STATE_QUERY (0x6D/0x6E) | Done |
| SERVO_CONFIG / servo write | Done (MVP) |
| I2C config/write/read | Done (MVP) |

## GUI

- Pin console table (Uno/Mega full range)
- Monitor tab: `AnalogSamples` preview, `StreamLog`
- I2C tab
- Diagram overlay from `PinStatusJson`

## Baud

Default **57600** (`runtimeBaud` in manifest).

## Testing

- Unit: `ctest -R ArduinoFirmataClient`, `ArduinoPinMap`, `ArduinoSampleBuffer`
- Hardware: Uno/Mega D13, A0, PWM D9 — see [FirmataTechDebt.md](FirmataTechDebt.md) TD-001
