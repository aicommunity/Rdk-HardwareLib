# Tier C/D modules (P2+)

## Status

| Slice | Host | Modules | Status |
|-------|------|---------|--------|
| I2C climate | `nmsdk_i2c_hub_v1` | `bme280` (`runtime=hub`) | **P2 first slice** (firmware + plugin) |
| ToF / IMU / power / PWM | planned hosts | `vl53l0x`, `mpu_6050`, `icm_20948`, `ina219`, `pca9685_16_ch_pwm`, `ads1115_adc` | `runtime=planned` |
| Wireless / displays | product-specific | `nrf24l01plus`, `lora_sx1278`, `oled_ssd1306_096`, … | `runtime=planned` |

## I2C hub (`nmsdk_i2c_hub_v1`)

- Sketch: [`Firmware/nmsdk_i2c_hub/`](../Firmware/nmsdk_i2c_hub/)
- Plugin: `UNmsdkI2cHubProtocolPlugin` (`nmsdk_i2c_hub_v1`)
- Frame `0x01`: `t`, `h`, `pressure_hpa` (BME280)
- Sample: `Bin/Configs/SpikeSamples/Hardware/15-I2cHub-BME280`

Next I2C upgrades: BMP280/BME680 on same hub, then VL53 / IMU dedicated sketches.

## Upgrade path (each module)

Catalog `runtime` → firmware/plugin → unit → optional HW test → Component doc.
