# nmsdk_pixel_hub_v1

WS2812 + MAX7219 (+ optional TFT/EPD MVP).

Commands: `LED SET/FILL/SHOW`, `MATRIX CLEAR/TEXT`, `TFT FILL/TEXT`, `EPD CLEAR/TEXT`, `PING`, `PROTO 2`.

Frame `0x41`: `led_count`, `last_ack`. Baud 57600. Host `nmsdk_pixel_hub_v1`.

Flags default: WS2812=1, MAX7219=1, ST7735/ILI9341/EPD154=0. EPD stays `planned` in catalog unless MVP ack path verified on hardware.
