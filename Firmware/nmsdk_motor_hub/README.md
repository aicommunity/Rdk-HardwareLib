# Nmsdk Motor Hub

Tier C firmware for Motor Shield DIR/PWM channel A with framed status reports and host watchdog.

- Sketch: `nmsdk_motor_hub.ino`
- Catalog: `nmsdk_motor_hub_v1`
- Host plugin: `nmsdk_motor_hub_v1`
- Commands: `MOTOR A <0..255>`, `MOTOR A DIR <0|1>`, `MOTOR STOP`, `SET PIN …`, `WATCHDOG <ms>`, `PING`, `PROTO 2`
- Protocol: [Docs/Hub-Protocols.md](../../Docs/Hub-Protocols.md)
- Build: `Scripts/build_arduino_firmware.ps1` / CI workflow `hardware-catalog.yml`
