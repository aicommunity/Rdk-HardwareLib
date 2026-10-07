# Tier C/D modules (P2+)

Priority I2C hosts (catalog `runtime=planned` until dedicated hub/plugin):

| id | Interface | Next host |
|----|-----------|-----------|
| `bme280` / `bmp280` / `bme680` | I2C | sensor I2C hub |
| `vl53l0x` / `vl53l1x` | I2C | ToF hub |
| `mpu_6050` / `icm_20948` | I2C | IMU hub |
| `ina219` | I2C | power monitor |
| `pca9685_16_ch_pwm` | I2C | PWM expander |
| `ads1115_adc` | I2C | ADC expander |

Wireless / displays (separate hosts by product need): `nrf24l01plus`, `lora_sx1278`, `oled_ssd1306_096`, `lcd_1602_i2c`, `ws2812b_neopixel`.

Each upgrade path: Catalog `runtime` → firmware/plugin → unit → optional HW test → Component doc.
