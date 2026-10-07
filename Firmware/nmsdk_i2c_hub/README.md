# nmsdk_i2c_hub_v1

P2 Tier C I2C hub: **BME280**, **VL53L0X**, **MPU6050**, **INA219**, **PCA9685**.

| Type | Payload floats | Module |
|------|----------------|--------|
| `0x01` | t, h, pressure_hpa | `bme280` |
| `0x30` | distance_mm | `vl53l0x` |
| `0x31` | ax..gz | `mpu_6050` |
| `0x32` | bus_v, current_ma, power_mw | `ina219` |
| `0x33` | ch, duty | `pca9685_16_ch_pwm` (after `SET PWM`) |

- Baud **57600**
- Host plugin: `nmsdk_i2c_hub_v1`
- Commands: `PROTO 2`, `PING`, `START/STOP READING`, `SET DELAY`, `SET PWM <ch> <0-4095>`
- Disable sensors at compile time with `-DNMSDK_I2C_HUB_*=0`

Libraries (Arduino Library Manager): Adafruit BME280, VL53L0X, MPU6050, INA219, PWM Servo Driver (+ BusIO / Unified Sensor as needed).

`icm_20948` remains `planned` (separate driver slice); MPU6050 covers the priority IMU slot.
