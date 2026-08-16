/*
 * Framed v2 hub template for Nmsdk custom firmware.
 * Frame: 0xAA | type | len LE | payload | crc8Maxim
 * Host: ArduinoCustomFirmware + protocol plugin, or sensor_lab_v1 plugin.
 */
#ifndef FRAMED_V2_HUB_INO
#define FRAMED_V2_HUB_INO

#include <Arduino.h>

static const uint8_t kSync = 0xAA;

static uint8_t crc8Maxim(const uint8_t* data, size_t len)
{
  uint8_t crc = 0;
  for (size_t i = 0; i < len; ++i) {
    uint8_t inbyte = data[i];
    for (uint8_t j = 0; j < 8; ++j) {
      uint8_t mix = (crc ^ inbyte) & 0x01;
      crc >>= 1;
      if (mix)
        crc ^= 0x8C;
      inbyte >>= 1;
    }
  }
  return crc;
}

static void sendFrame(uint8_t type, const uint8_t* payload, uint16_t len)
{
  uint8_t hdr[4] = {kSync, type, (uint8_t)(len & 0xFF), (uint8_t)((len >> 8) & 0xFF)};
  Serial.write(hdr, 4);
  if (len)
    Serial.write(payload, len);
  uint8_t crc_buf[3 + 256];
  crc_buf[0] = type;
  crc_buf[1] = hdr[2];
  crc_buf[2] = hdr[3];
  for (uint16_t i = 0; i < len && i < 256; ++i)
    crc_buf[3 + i] = payload[i];
  Serial.write(crc8Maxim(crc_buf, 3 + len));
}

void setup()
{
  Serial.begin(57600);
}

void loop()
{
  // TODO: parse line commands; emit typed frames.
  delay(20);
}

#endif
