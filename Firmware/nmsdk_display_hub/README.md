# nmsdk_display_hub_v1

I2C text displays: SSD1306 OLED, LCD 1602/2004.

| Command | Effect |
|---------|--------|
| `CLEAR` / `PRINT r c text` | LCD |
| `OLED CLEAR` / `OLED PRINT text` | SSD1306 |
| `PING` / `PROTO 2` | standard |

Frames: `0x7F` pong; `0x40` status (`rows`,`cols`,`driver_id`).

Flags: `NMSDK_DISPLAY_HUB_SSD1306` (1), `_LCD1602` (1), `_LCD2004` (0). Baud 57600. Host plugin `nmsdk_display_hub_v1`.
