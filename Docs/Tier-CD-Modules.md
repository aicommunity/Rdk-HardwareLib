# Tier C/D modules (P2+)

## Status

| Slice | Host | Modules | Status |
|-------|------|---------|--------|
| I2C climate | `nmsdk_i2c_hub_v1` | `bme280` | **done** (`0x01`) |
| I2C ToF | same | `vl53l0x` | **done** (`0x30`); `vl53l1x` still planned |
| I2C IMU | same | `mpu_6050` | **done** (`0x31`); `icm_20948` planned |
| I2C power | same | `ina219` | **done** (`0x32`) |
| I2C PWM | same | `pca9685_16_ch_pwm` | **done** (`SET PWM` + `0x33`) |
| Sensor 1-wire/DHT | `nmsdk_sensor_hub_v1` | `dht22`, `ds18b20` | **done** |
| ADC expander | planned | `ads1115_adc` | planned |
| UART product (Nextion/BT/GSM) | — | `nextion_hmi`, `hc_05`/`hc_06`, `hm_10`, `sim800l`, `a9g` | **planned** (Wave 0: false `hub` cleared; Wave 5 host) |
| Wireless / displays | product-specific | OLED, LoRa, NRF, … | planned |

## Catalog hygiene (Wave 0)

Invariant (unit `Test_HardwareModulesCatalog`): `runtime=hub|motor_hub` ⇒ non-empty `preferredFirmware` pointing at firmware JSON with non-empty `hostPlugin`.

Modules formerly tagged `hub` without binding were returned to `planned` until `nmsdk_uart_device_hub_v1` lands.

## I2C hub (`nmsdk_i2c_hub_v1`)

- Sketch: [`Firmware/nmsdk_i2c_hub/`](../Firmware/nmsdk_i2c_hub/)
- Plugin: `UNmsdkI2cHubProtocolPlugin`
- Sample: `Bin/Configs/SpikeSamples/Hardware/15-I2cHub-BME280` (same ClassName for all I2C hub sensors)
- Docs: [Hub-Protocols.md](Hub-Protocols.md)

Compile flags: `NMSDK_I2C_HUB_BME280`, `_VL53`, `_MPU6050`, `_INA219`, `_PCA9685` (default 1).

## Upgrade path (remaining)

Catalog `runtime` → firmware/plugin → unit → optional HW test → Component doc.
