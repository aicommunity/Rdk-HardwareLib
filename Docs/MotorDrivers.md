# Motor Drivers

## RU

Сравнение драйверов / шилдов: [`Docs/ArduinoShields/arduino-motor-shields-comparison.xlsx`](ArduinoShields/arduino-motor-shields-comparison.xlsx).

Импорт → `Catalog/shields/<id>.json` (+ module entry для non-shield).

| controlModel | Смысл |
|--------------|--------|
| `dir_pwm` | DIR + PWM (+ optional brake/sense) — Motor Shield R3 |
| `dir2_pwm` | IN1 + IN2 + PWM/enable — L298N, Seeed Motor Shield V1 |
| `step_dir` | STEP + DIR — A4988 / TMC |
| `i2c` | I2C motor driver |

Канон P0 wheeled: `motor_shield_r3` + firmware `nmsdk_motor_hub_v1`. `wire_l298n` uses A: D2/D4/D5 and B: D7/D8/D6 (IN1/IN2/ENA); remove the ENA/ENB jumpers before using PWM. The motor hub sends 0x21 pin maps in a 12-byte format with 255 for an unassigned role; the host still accepts the previous 8-byte frame.

## EN

Motor driver catalog from the comparison xlsx. Motor Shield R3 uses `dir_pwm`; L298N and Seeed Motor Shield V1 use `dir2_pwm`. The motor hub accepts legacy 8-byte and current 12-byte pin-map frames.
