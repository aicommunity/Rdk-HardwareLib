# Tier C/D modules (P2+)

## Status

| Slice | Host | Modules | Status |
|-------|------|---------|--------|
| I2C climate | `nmsdk_i2c_hub_v1` | `bme280` | **done** (`0x01`) |
| I2C ToF | same | `vl53l0x` | **done** (`0x30`) |
| I2C IMU | same | `mpu_6050` | **done** (`0x31`) |
| I2C power / PWM | same | `ina219`, `pca9685_16_ch_pwm` | **done** (`0x32`/`0x33`) |
| I2C extras 1a | same | `bmp280`, `bme680`, `vl53l1x`, `icm_20948` | **done** (`0x34`–`0x36`) |
| I2C extras 1b | same | `aht20`, `sht31`, `bh1750`, `mlx90614`, `sgp30` | **done** (`0x37`–`0x3A`) |
| I2C extras 1c | same | `tcs34725`, `adxl345`, `apds_9960`, `ads1115_adc` | **done** (`0x3B`–`0x3E`) |
| Sensor 1-wire/DHT | `nmsdk_sensor_hub_v1` | `dht22`, `ds18b20` | **done** |
| UART product | Wave 5 | Nextion/BT/GSM | planned (false hub cleared Wave 0) |
| Display / pixel / radio | Waves 2–4 | OLED/LCD, WS2812, NRF… | planned |

## Catalog hygiene (Wave 0)

Invariant: `runtime=hub|motor_hub` ⇒ `preferredFirmware` with firmware `hostPlugin`.

## I2C hub (`nmsdk_i2c_hub_v1`)

- Sketch: [`Firmware/nmsdk_i2c_hub/`](../Firmware/nmsdk_i2c_hub/)
- Plugin: `UNmsdkI2cHubProtocolPlugin`
- Sample: `Bin/Configs/SpikeSamples/Hardware/15-I2cHub-BME280`
- Docs: [Hub-Protocols.md](Hub-Protocols.md)
