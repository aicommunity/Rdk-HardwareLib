# Modules Catalog

## RU

Каталог модулей Arduino/ESP32 импортирован из
[`Docs/ArduinoShields/arduino-esp32-modules-catalog.xlsx`](ArduinoShields/arduino-esp32-modules-catalog.xlsx)
скриптом [`Scripts/import_modules_catalog_xlsx.py`](../Scripts/import_modules_catalog_xlsx.py).

JSON: `Catalog/modules/<id>.json`. Индекс: `Catalog/catalog.json`.
Зеркало runtime: `Bin/HardwareCatalog` (`Scripts/sync_hardware_catalog.py`).

Поле `runtime`: `firmata` | `hub` | `motor_hub` | `planned`.

Перегенерация TOC:

```bash
cd Libraries/Rdk-HardwareLib/Scripts
python3 import_modules_catalog_xlsx.py
python3 generate_modules_catalog_toc.py
python3 sync_hardware_catalog.py --dest /path/to/Bin/HardwareCatalog
```

### TOC по Category

#### Датчики (28)

| id | title | runtime |
|----|-------|---------|
| `acs712` | ACS712 | `firmata` |
| `adxl345` | ADXL345 | `planned` |
| `aht20` | AHT20 | `planned` |
| `apds_9960` | APDS-9960 | `planned` |
| `bh1750` | BH1750 | `planned` |
| `bme280` | BME280 | `hub` |
| `bme680` | BME680 | `planned` |
| `bmp280` | BMP280 | `planned` |
| `dht11` | DHT11 | `firmata` |
| `dht22` | DHT22 | `hub` |
| `ds18b20` | DS18B20 | `hub` |
| `hc_sr04` | HC-SR04 Ultrasonic | `firmata` |
| `hc_sr501_pir` | HC-SR501 PIR | `firmata` |
| `hx711_plus_тензодатчик` | HX711 + тензодатчик | `firmata` |
| `icm_20948` | ICM-20948 | `planned` |
| `ina219` | INA219 | `hub` |
| `ldr` | LDR (photoresistor) | `firmata` |
| `mlx90614` | MLX90614 | `planned` |
| `mpu_6050` | MPU-6050 | `hub` |
| `mq_135` | MQ-135 | `firmata` |
| `sgp30` | SGP30 | `planned` |
| `sht31` | SHT31 | `planned` |
| `soil_moisture` | Soil moisture | `firmata` |
| `tcs34725` | TCS34725 | `planned` |
| `vl53l0x` | VL53L0X | `hub` |
| `vl53l1x` | VL53L1X | `planned` |
| `датчик_влажности_почвы` | Датчик влажности почвы | `firmata` |
| `фоторезистор_ldr_модуль` | Фоторезистор LDR модуль | `firmata` |

#### Шилды / расширения (12)

| id | title | runtime |
|----|-------|---------|
| `ads1115_adc` | ADS1115 ADC | `planned` |
| `arduino_motor_shield_l298` | Arduino Motor Shield (L298) | `firmata` |
| `ds3231_rtc` | DS3231 RTC | `firmata` |
| `ethernet_shield_w5500` | Ethernet Shield W5500 | `firmata` |
| `gps_neo_6m` | GPS NEO-6M | `firmata` |
| `mcp23017_i` | MCP23017 I | `firmata` |
| `mcp2515_can_модуль` | MCP2515 CAN модуль | `firmata` |
| `pca9685_16_ch_pwm` | PCA9685 16-ch PWM | `hub` |
| `pcf8574_lcd_backpack` | PCF8574 LCD backpack | `firmata` |
| `proto_shield` | Proto Shield | `firmata` |
| `relay_shield` | Relay Shield | `firmata` |
| `tca9548a_i2c_mux` | TCA9548A I2C mux | `firmata` |

#### Дисплеи (10)

| id | title | runtime |
|----|-------|---------|
| `e_paper_waveshare_154` | e-Paper Waveshare 1.54" | `planned` |
| `lcd_1602_i2c` | LCD 1602 I2C | `planned` |
| `lcd_2004_i2c` | LCD 2004 I2C | `planned` |
| `max7219_88_led_matrix` | MAX7219 8×8 LED matrix | `planned` |
| `nextion_hmi` | Nextion HMI | `planned` |
| `oled_ssd1306_096` | OLED SSD1306 0.96" | `planned` |
| `st7735_18_tft` | ST7735 1.8" TFT | `planned` |
| `tft_ili9341_2428` | TFT ILI9341 2.4–2.8" | `planned` |
| `tm1637_4_digit` | TM1637 4-digit | `firmata` |
| `ws2812b_neopixel` | WS2812B NeoPixel | `planned` |

#### Беспроводная связь (9)

| id | title | runtime |
|----|-------|---------|
| `a9g_gsmplusgps` | A9G GSM+GPS | `planned` |
| `bluetooth_ble_hm_10` | Bluetooth BLE HM-10 | `planned` |
| `esp32_wifi` | ESP32 Wi‑Fi | `planned` |
| `hc_05_bluetooth_classic` | HC-05 Bluetooth Classic | `planned` |
| `hc_06_bluetooth` | HC-06 Bluetooth | `planned` |
| `lora_sx1278` | LoRa SX1278 | `planned` |
| `m_433_мгц_fs1000a` | 433 МГц FS1000A | `firmata` |
| `nrf24l01plus` | nRF24L01+ | `planned` |
| `sim800l_gsm` | SIM800L GSM | `planned` |

#### Управление / ввод (8)

| id | title | runtime |
|----|-------|---------|
| `analog_joystick` | Analog Joystick | `firmata` |
| `membrane_keypad_44` | Membrane Keypad 4×4 | `firmata` |
| `pn532_nfc` | PN532 NFC | `planned` |
| `relay` | Relay | `firmata` |
| `rfid_rc522` | RFID RC522 | `planned` |
| `rotary_encoder_ky_040` | Rotary Encoder KY-040 | `firmata` |
| `tm1638_ledpluskeys` | TM1638 LED+Keys | `firmata` |
| `touch_sensor_ttp223` | Touch Sensor TTP223 | `firmata` |

#### Компоненты роботов (6)

| id | title | runtime |
|----|-------|---------|
| `a4988_stepper_driver` | A4988 Stepper Driver | `firmata` |
| `drv8825_stepper_driver` | DRV8825 Stepper Driver | `firmata` |
| `ir_line_tracker` | IR Line Tracker | `firmata` |
| `l298n_motor_driver` | L298N Motor Driver | `firmata` |
| `servo_sg90` | Servo SG90 | `firmata` |
| `tb6612fng_motor_driver` | TB6612FNG Motor Driver | `firmata` |

#### Прочее (6)

| id | title | runtime |
|----|-------|---------|
| `button` | Push Button | `firmata` |
| `dc_motor_channel` | DC Motor Channel | `planned` |
| `led` | LED | `firmata` |
| `potentiometer` | Potentiometer | `firmata` |
| `pwm_led` | PWM LED | `firmata` |
| `servo` | Servo | `planned` |

#### Моторы / драйверы (1)

| id | title | runtime |
|----|-------|---------|
| `wire_l298n` | L298N dual H-bridge module | `motor_hub` |

## EN

Module JSON catalog generated from the xlsx sheet «Каталог».
See RU section for paths, runtime badges, and the category TOC.
