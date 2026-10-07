# Tier C/D modules (P2+)

## Status

| Slice | Host | Modules | Status |
|-------|------|---------|--------|
| I2C priority | `nmsdk_i2c_hub_v1` | bme280, vl53l0x, mpu6050, ina219, pca9685 | **done** |
| I2C extras | same | bmp280…ads1115_adc (0x34–0x3E) | **done** |
| Sensor hub | `nmsdk_sensor_hub_v1` | dht22, ds18b20 | **done** |
| Display | `nmsdk_display_hub_v1` | oled_ssd1306_096, lcd_1602/2004 | **done** |
| Pixel | `nmsdk_pixel_hub_v1` | ws2812, max7219, st7735, ili9341 | **done** (e-paper planned) |
| Radio | `nmsdk_radio_hub_v1` | nrf24, lora, rc522, pn532, esp32_wifi | **done** |
| UART product | `nmsdk_uart_device_hub_v1` | nextion, hc05/06, hm10, sim800, a9g | **done** |

## Catalog hygiene

Invariant: `runtime=hub|motor_hub` ⇒ `preferredFirmware` with firmware `hostPlugin`. E-paper stays `planned` until EPD MVP verified on hardware.

## Docs

[Hub-Protocols.md](Hub-Protocols.md), firmware READMEs under `Firmware/nmsdk_*_hub/`.
