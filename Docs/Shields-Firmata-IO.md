# Shields → Firmata I/O

## Sensor Shield V5

Порты `Dx`/`Ax` → DeviceIO `Port`. Пресет Firmata: `sensor_shield_inputs_A0_A5`.

## Motor Shield Rev3

Каналы A/B: DIR/PWM/BRAKE/SENSE из catalog `motor_shield_r3`. Пресет: `motor_shield_r3_coast`.

## Seeed Motor Shield V1

Иные пины (DIR/DIR2/PWM/EN). Пресет: `motor_shield_seeed_enable`.

## ApplyHardwareSetup

Edge на `ArduinoFirmata`: читает `HardwareSetupPath`/`HardwareSetupJson`, строит `PinConfigBatch`, результат в `LastSetupApplyResult`.
