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

### Sensor (`nmsdk_sensor_hub_v1`)

- `PROTO 2`, `PING`, `START READING` / `STOP READING`, `SET DELAY <ms>`
- `SET DEVICE dht|trig|echo|hall <Dn|An>` — runtime pin bind (not flash-only)
- `CLEAR DEVICES`, `GET PINS`
- DHT22: compile with `-DDHTTYPE=DHT22` (default DHT11). DS18B20 reserved (`NMSDK_SENSOR_HUB_DS18B20`) for Tier B follow-up.

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
| `nmsdk_motor_hub_v1` | `nmsdk_motor_hub_v1` | SpikeSamples/Hardware/15-MotorHub |

See also [Protocol-Plugins.md](Protocol-Plugins.md), [WheeledRobots.md](WheeledRobots.md), [Firmware/README.md](../Firmware/README.md).

Waveshare UGV JSON (WaveRover) is **not** this hub — see [Protocol-WaveshareUgvJson.md](Protocol-WaveshareUgvJson.md).
