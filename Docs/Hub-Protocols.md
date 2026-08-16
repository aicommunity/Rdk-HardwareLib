# Nmsdk Hub Protocols (framed v2)

## Shared framing

`0xAA | type | len_lo | len_hi | payload | crc8Maxim(type,len,payload)`

CRC matches `sensor_lab` / `framed_v2_hub` template.

| type | Hub | Payload |
|------|-----|---------|
| `0x01` | sensor | sensors: `[0, n] + n×float LE` (t, h, distance_cm, hall) |
| `0x20` | motor | `[ch, pwm, dir] + float sense` |
| `0x21` | motor | pin map `[dir,pwm,brake,sense]` as MCU pin numbers |
| `0x22` | sensor | pin map `[dht,trig,echo,hall]` |
| `0x7F` | both | ping/pong |

## Line commands (host → board)

### Sensor (`nmsdk_sensor_hub_v1`)

- `PROTO 2`, `PING`, `START READING` / `STOP READING`, `SET DELAY <ms>`
- `SET DEVICE dht|trig|echo|hall <Dn|An>` — runtime pin bind (not flash-only)
- `CLEAR DEVICES`, `GET PINS`

### Motor (`nmsdk_motor_hub_v1`)

- `PROTO 2`, `PING`, `MOTOR A <0..255>`, `MOTOR A DIR <0|1>`, `MOTOR STOP`
- `SET PIN dir|pwm|brake|sense <Dn|An>`
- `WATCHDOG <ms>` — auto `safeStop` if no host command within ms while PWM>0 (default 2000; `0` disables)
- `GET PINS`

## Host plugins

| Catalog firmware | Plugin id | Sample |
|------------------|-----------|--------|
| `nmsdk_sensor_hub_v1` | `nmsdk_sensor_hub_v1` | SpikeSamples/Hardware/14-SensorHub |
| `nmsdk_motor_hub_v1` | `nmsdk_motor_hub_v1` | SpikeSamples/Hardware/15-MotorHub |

See also [Protocol-Plugins.md](Protocol-Plugins.md), [Firmware/README.md](../Firmware/README.md).
