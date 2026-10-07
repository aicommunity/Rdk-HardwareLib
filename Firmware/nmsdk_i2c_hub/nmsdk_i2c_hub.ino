/*
 * nmsdk_i2c_hub_v1 — framed v2 I2C sensor hub (P2 Tier C).
 * Host plugin: nmsdk_i2c_hub_v1
 *
 * Frame 0x01: [0, n] + n×float LE — t_c, humidity_pct, pressure_hpa (BME280)
 * Build: Adafruit BME280 + Adafruit BusIO (+ Adafruit Unified Sensor)
 * Optional: -DNMSDK_I2C_HUB_BME280=0 to skip BME (zeros)
 */
#include <Arduino.h>
#include <Wire.h>

#ifndef NMSDK_I2C_HUB_BAUD
#define NMSDK_I2C_HUB_BAUD 57600
#endif
#ifndef NMSDK_I2C_HUB_BME280
#define NMSDK_I2C_HUB_BME280 1
#endif

#if NMSDK_I2C_HUB_BME280
#include <Adafruit_BME280.h>
Adafruit_BME280 bme;
bool bmeOk = false;
#endif

bool readingEnabled = true;
uint8_t protocolVersion = 2;
unsigned long mainDelay = 500;

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
  if (len > 0)
    Serial.write(payload, len);
  uint8_t frame[4 + 64];
  int frameLen = 4;
  memcpy(frame, header, 4);
  if (len > 0 && len <= 64) {
    memcpy(frame + 4, payload, len);
    frameLen += len;
  }
  Serial.write(crc8Maxim(frame, frameLen));
}

void sendSensorData()
{
  float t = 0, h = 0, p = 0;
#if NMSDK_I2C_HUB_BME280
  if (bmeOk) {
    t = bme.readTemperature();
    h = bme.readHumidity();
    p = bme.readPressure() / 100.0f;
  }
#endif
  uint8_t body[2 + 3 * sizeof(float)];
  body[0] = 0;
  body[1] = 3;
  memcpy(body + 2, &t, 4);
  memcpy(body + 6, &h, 4);
  memcpy(body + 10, &p, 4);
  writeFramedV2(0x01, body, sizeof(body));
}

void sendPong()
{
  uint8_t one = 1;
  writeFramedV2(0x7F, &one, 1);
}

void setup()
{
  Serial.begin(NMSDK_I2C_HUB_BAUD);
  Wire.begin();
#if NMSDK_I2C_HUB_BME280
  bmeOk = bme.begin(0x76) || bme.begin(0x77);
  if (!bmeOk)
    Serial.println(F("BME280 not found"));
#endif
}

void loop()
{
  if (Serial.available() > 0) {
    String command = Serial.readStringUntil('\n');
    command.trim();
    if (command == "START READING")
      readingEnabled = true;
    else if (command == "STOP READING")
      readingEnabled = false;
    else if (command == "PING" || command == "GET STATUS")
      sendPong();
    else if (command.startsWith("PROTO ")) {
      int ver = command.substring(6).toInt();
      protocolVersion = (ver >= 2) ? 2 : 1;
      Serial.println(ver >= 2 ? F("PROTO OK 2") : F("PROTO OK 1"));
    } else if (command.startsWith("SET DELAY")) {
      int d = command.substring(10).toInt();
      if (d > 0)
        mainDelay = d;
    }
  }
  if (readingEnabled)
    sendSensorData();
  delay(mainDelay);
}
