# nmsdk_radio_hub_v1

AVR radio/RFID hub: NRF24 + RC522 (default). Optional LoRa / PN532 (xor RC522).

Commands: `RADIO SEND <hex>`, `PING`, `PROTO 2`.

Frames: `0x50` RX meta (`rx_len`,`rssi`); `0x51` UID; `0x7F` pong.

LoRa (`_LORA=1`, sandeepmistry LoRa lib) and PN532 (`_PN532=1`, xor RC522) implemented behind flags.

ESP32 Wi‑Fi twin: `Firmware/nmsdk_radio_hub_esp32/` (frame `0x52`). Host plugin `nmsdk_radio_hub_v1`.
