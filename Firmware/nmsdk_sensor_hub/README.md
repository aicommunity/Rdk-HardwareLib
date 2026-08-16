# Nmsdk Sensor Hub

Tier C firmware for timing-sensitive sensors (DHT, HC-SR04) that Standard Firmata cannot host reliably.

- Sketch: `nmsdk_sensor_hub.ino`
- Catalog: `nmsdk_sensor_hub_v1` (`available: true`)
- Host plugin: `nmsdk_sensor_hub_v1`
- Runtime pin bind: `SET DEVICE dht|trig|echo|hall <Dn|An>`, `CLEAR DEVICES`, `GET PINS`
- Protocol: [Docs/Hub-Protocols.md](../../Docs/Hub-Protocols.md)
- Build: `Scripts/build_arduino_firmware.ps1` / CI `hardware-catalog.yml`
