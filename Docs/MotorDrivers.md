# Motor Drivers

## RU

Сравнение драйверов / шилдов: [`Docs/ArduinoShields/arduino-motor-shields-comparison.xlsx`](ArduinoShields/arduino-motor-shields-comparison.xlsx).

Импорт → `Catalog/shields/<id>.json` (+ module entry для non-shield).

| controlModel | Смысл |
|--------------|--------|
| `dir_pwm` | DIR + PWM (+ optional brake/sense) — Motor Shield R3, L298N, TB6612 |
| `step_dir` | STEP + DIR — A4988 / TMC |
| `i2c` | I2C motor driver |

Канон P0 wheeled: `motor_shield_r3` + firmware `nmsdk_motor_hub_v1`.

## EN

Motor driver catalog from the comparison xlsx. P0 wheeled default is Motor Shield R3 / `dir_pwm`.
