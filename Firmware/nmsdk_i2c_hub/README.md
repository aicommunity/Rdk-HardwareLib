# nmsdk_i2c_hub_v1

P2 / P2+ I2C hub: priority BME280, VL53L0X, MPU6050, INA219, PCA9685 plus optional climate/env/color/ADC sensors.

| Type | Payload floats | Module(s) | Flag (default) |
|------|----------------|-----------|----------------|
| `0x01` | t, h, pressure_hpa | `bme280` | `NMSDK_I2C_HUB_BME280` (1) |
| `0x30` | distance_mm | `vl53l0x` | `_VL53` (1) |
| `0x31` | ax..gz | `mpu_6050` | `_MPU6050` (1) |
| `0x32` | bus_v, shunt_v, current_ma, power_mw | `ina219` | `_INA219` (1) |
| `0x33` | ch, duty | `pca9685_16_ch_pwm` | `_PCA9685` (1) |
| `0x34` | t, h, pressure_hpa | `bmp280`, `bme680` | `_BMP280` / `_BME680` (0) |
| `0x35` | distance_mm | `vl53l1x` | `_VL53L1` (0) |
| `0x36` | ax..gz[,mx,my] | `icm_20948` | `_ICM20948` (0) |
| `0x37` | t, h | `aht20`, `sht31` | `_AHT20` / `_SHT31` (0) |
| `0x38` | lux | `bh1750` | `_BH1750` (0) |
| `0x39` | object_c, ambient_c | `mlx90614` | `_MLX90614` (0) |
| `0x3A` | eco2, tvoc | `sgp30` | `_SGP30` (0) |
| `0x3B` | r,g,b,c | `tcs34725` | `_TCS34725` (0) |
| `0x3C` | ax,ay,az | `adxl345` | `_ADXL345` (0) |
| `0x3D` | gesture, prox, r,g,b | `apds_9960` | `_APDS9960` (0) |
| `0x3E` | ch0..ch3 V | `ads1115_adc` | `_ADS1115` (0) |

- Baud **57600**
- Host plugin: `nmsdk_i2c_hub_v1`
- Commands: `PROTO 2`, `PING`, `START/STOP READING`, `SET DELAY`, `SET PWM <ch> <0-4095>`

Libraries (when flag=1): Adafruit BME280/BMP280/BME680/VL53L0X/VL53L1X/MPU6050/ICM20948/INA219/PWM Servo/AHTX0/SHT31/MLX90614/SGP30/TCS34725/ADXL345/APDS9960/ADS1X15; BH1750 (claws).
