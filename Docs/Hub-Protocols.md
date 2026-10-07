# Nmsdk Hub Protocols (framed v2)

## Shared framing

`0xAA | type | len_lo | len_hi | payload | crc8Maxim(type,len,payload)`

CRC matches `sensor_lab` / `framed_v2_hub` template.

| type | Hub | Payload |
|------|-----|---------|
| `0x01` | sensor | sensors: `[0, n] + n×float LE` (t, h, distance_cm, hall) |
| `0x20` | motor | `[ch, pwm, dir] + float sense` (ch 0=A/left, 1=B/right; host publishes `left_*`/`right_*`) |
| `0x21` | motor | pin map 8 bytes `A(dir,pwm,brake,sense)+B(...)`; 4-byte legacy A-only still accepted |
| `0x22` | sensor | pin map `[dht,trig,echo,hall]` |
| `0x7F` | both | ping/pong |

## Line commands (host → board)

### Sensor (`nmsdk_sensor_hub_v1` / `nmsdk_sensor_hub_esp32_v1`)

- `PROTO 2`, `PING`, `START READING` / `STOP READING`, `SET DELAY <ms>`
- `SET DEVICE dht|trig|echo|hall|ds <Dn|An>` — runtime pin bind (not flash-only)
- `CLEAR DEVICES`, `GET PINS`
- DHT22: compile with `-DDHTTYPE=DHT22` (default DHT11)
- DS18B20: `-DNMSDK_SENSOR_HUB_DS18B20=1` (+ OneWire, DallasTemperature); 5th float `ds18b20` in frame `0x01`
- ESP32 twin sketch: `Firmware/nmsdk_sensor_hub_esp32/` @ baud **115200**, same host plugin id

### Motor (`nmsdk_motor_hub_v1`)

- `PROTO 2`, `PING`, `MOTOR A|B <0..255>`, `MOTOR A|B DIR <0|1>`, `MOTOR STOP`
- `SET PIN [A|B] dir|pwm|brake|sense <Dn|An>` (legacy without channel → A)
- `WATCHDOG <ms>` — auto `safeStop` both channels if no host command within ms while any PWM>0 (default 2000; `0` disables)
- `GET PINS`
- ESP32 twin sketch: `Firmware/nmsdk_motor_hub_esp32/` / catalog `nmsdk_motor_hub_esp32_v1` (baud 115200)

## Host plugins

| Catalog firmware | Plugin id | Sample |
|------------------|-----------|--------|
| `nmsdk_sensor_hub_v1` | `nmsdk_sensor_hub_v1` | SpikeSamples/Hardware/14-SensorHub |
| `nmsdk_motor_hub_v1` | `nmsdk_motor_hub_v1` | SpikeSamples/Hardware/09–11 wheeled |
| `nmsdk_i2c_hub_v1` | `nmsdk_i2c_hub_v1` | SpikeSamples/Hardware/15-I2cHub-BME280 |

### I2C (`nmsdk_i2c_hub_v1`)

- `PROTO 2`, `PING`, `START READING` / `STOP READING`, `SET DELAY <ms>`
- `SET PWM <ch0-15> <0-4095>` — PCA9685 duty (echo frame `0x33`)
- Frames:

| type | floats | Module |
|------|--------|--------|
| `0x01` | t, h, pressure_hpa | `bme280` |
| `0x30` | distance_mm | `vl53l0x` |
| `0x31` | ax,ay,az,gx,gy,gz | `mpu_6050` |
| `0x32` | bus_v, shunt_v, current_ma, power_mw | `ina219` |
| `0x33` | ch, duty | PCA9685 echo |
| `0x34` | t, h, pressure_hpa | `bmp280`, `bme680` |
| `0x35` | distance_mm | `vl53l1x` |
| `0x36` | ax..gz[,mx,my] | `icm_20948` |
| `0x37` | t, h | `aht20`, `sht31` |
| `0x38` | lux | `bh1750` |
| `0x39` | object_c, ambient_c | `mlx90614` |
| `0x3A` | eco2, tvoc | `sgp30` |
| `0x3B` | r,g,b,c | `tcs34725` |
| `0x3C` | ax,ay,az | `adxl345` |
| `0x3D` | gesture, prox, r,g,b | `apds_9960` |
| `0x3E` | ch0..ch3 V | `ads1115_adc` |

See [Tier-CD-Modules.md](Tier-CD-Modules.md), `Firmware/nmsdk_i2c_hub/README.md`.

See also [Protocol-Plugins.md](Protocol-Plugins.md), [WheeledRobots.md](WheeledRobots.md), [Firmware/README.md](../Firmware/README.md).

Waveshare UGV JSON (WaveRover) is **not** this hub — see [Protocol-WaveshareUgvJson.md](Protocol-WaveshareUgvJson.md).

### Display (`nmsdk_display_hub_v1`)

- Commands: `CLEAR`, `PRINT <row> <col> <text>`, `OLED CLEAR`, `OLED PRINT <text>`, `PING`, `PROTO 2`
- Frames: `0x7F` pong; `0x40` status (`rows`,`cols`,`driver_id`)
- Sample: `16-DisplayHub`

### Pixel (`nmsdk_pixel_hub_v1`)

- Commands: `LED SET/FILL/SHOW`, `MATRIX CLEAR/TEXT`, `TFT FILL/TEXT`, `EPD CLEAR/TEXT`
- Frames: `0x7F`; `0x41` (`led_count`,`last_ack`)
- Sample: `17-PixelHub`

### Radio (`nmsdk_radio_hub_v1`)

- Commands: `RADIO SEND <hex>`, `PING`, `PROTO 2`
- Frames: `0x50` RX, `0x51` UID, `0x52` Wi‑Fi (ESP32 twin), `0x7F`
- Sample: `18-RadioHub`

### UART device (`nmsdk_uart_device_hub_v1`)

- Commands: `HMI TX`, `AT…`, `BRIDGE ON/OFF`, `PING`, `PROTO 2`
- Frames: `0x60` UTF-8 line; `0x7F`
- Sample: `19-UartDeviceHub`
