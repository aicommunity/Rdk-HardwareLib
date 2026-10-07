# nmsdk_uart_device_hub_v1

Second-UART bridge for Nextion / HC-05 / SIM800 / A9G.

- Host USB: `Serial` @ 57600 (framed)
- Device: `Serial1` on Mega/ESP32, else SoftwareSerial D10/D11 (best-effort on Uno)

Commands: `HMI TX <line>`, `AT…`, `BRIDGE ON/OFF`, `PING`, `PROTO 2`.

Frame `0x60`: UTF-8 device line. Host plugin `nmsdk_uart_device_hub_v1`.
