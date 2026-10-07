/*
 * nmsdk_radio_hub_esp32 — Wi‑Fi telemetry twin for catalog esp32_wifi.
 * Same host plugin nmsdk_radio_hub_v1; frame 0x52: rssi, ip0..ip3
 */
#include <Arduino.h>
#include <WiFi.h>

#ifndef NMSDK_RADIO_HUB_BAUD
#define NMSDK_RADIO_HUB_BAUD 115200
#endif
#ifndef NMSDK_WIFI_SSID
#define NMSDK_WIFI_SSID "nmsdk"
#endif
#ifndef NMSDK_WIFI_PASS
#define NMSDK_WIFI_PASS "nmsdknmsdk"
#endif

uint8_t crc8Maxim(const uint8_t* data, int len)
{
  uint8_t crc = 0;
  for (int i = 0; i < len; ++i) {
    crc ^= data[i];
    for (int b = 0; b < 8; ++b)
      crc = (crc & 1) ? (uint8_t)((crc >> 1) ^ 0x8C) : (uint8_t)(crc >> 1);
  }
  return crc;
}

void writeFramedV2(uint8_t type, const uint8_t* payload, uint16_t len)
{
  uint8_t header[4] = {0xAA, type, (uint8_t)(len & 0xFF), (uint8_t)((len >> 8) & 0xFF)};
  Serial.write(header, 4);
  if (len)
    Serial.write(payload, len);
  uint8_t frame[4 + 64];
  memcpy(frame, header, 4);
  int fl = 4;
  if (len && len <= 64) {
    memcpy(frame + 4, payload, len);
    fl += len;
  }
  Serial.write(crc8Maxim(frame, fl));
}

void writeFloats(uint8_t type, const float* values, uint8_t n)
{
  uint8_t body[2 + 8 * sizeof(float)];
  if (n > 8)
    n = 8;
  body[0] = 0;
  body[1] = n;
  memcpy(body + 2, values, n * sizeof(float));
  writeFramedV2(type, body, (uint16_t)(2 + n * sizeof(float)));
}

void sendWifi()
{
  IPAddress ip = WiFi.localIP();
  float v[5] = {(float)WiFi.RSSI(), (float)ip[0], (float)ip[1], (float)ip[2], (float)ip[3]};
  writeFloats(0x52, v, 5);
}

void setup()
{
  Serial.begin(NMSDK_RADIO_HUB_BAUD);
  WiFi.mode(WIFI_STA);
  WiFi.begin(NMSDK_WIFI_SSID, NMSDK_WIFI_PASS);
}

void loop()
{
  if (Serial.available() > 0) {
    String command = Serial.readStringUntil('\n');
    command.trim();
    if (command == "PING") {
      uint8_t one = 1;
      writeFramedV2(0x7F, &one, 1);
      sendWifi();
    } else if (command.startsWith("PROTO ")) {
      Serial.println(F("PROTO OK 2"));
    }
  }
  static unsigned long last = 0;
  if (millis() - last > 2000) {
    last = millis();
    if (WiFi.status() == WL_CONNECTED)
      sendWifi();
  }
}
