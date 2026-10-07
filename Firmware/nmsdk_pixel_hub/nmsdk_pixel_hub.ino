/*
 * nmsdk_pixel_hub_v1 — WS2812 / MAX7219 / TFT / EPD MVP.
 * Host: nmsdk_pixel_hub_v1. Frames: 0x7F pong; 0x41 led_count,last_ack
 */
#include <Arduino.h>
#include <SPI.h>

#ifndef NMSDK_PIXEL_HUB_BAUD
#define NMSDK_PIXEL_HUB_BAUD 57600
#endif
#ifndef NMSDK_PIXEL_HUB_WS2812
#define NMSDK_PIXEL_HUB_WS2812 1
#endif
#ifndef NMSDK_PIXEL_HUB_MAX7219
#define NMSDK_PIXEL_HUB_MAX7219 1
#endif
#ifndef NMSDK_PIXEL_HUB_ST7735
#define NMSDK_PIXEL_HUB_ST7735 0
#endif
#ifndef NMSDK_PIXEL_HUB_ILI9341
#define NMSDK_PIXEL_HUB_ILI9341 0
#endif
#ifndef NMSDK_PIXEL_HUB_EPD154
#define NMSDK_PIXEL_HUB_EPD154 0
#endif
#ifndef NMSDK_PIXEL_LED_PIN
#define NMSDK_PIXEL_LED_PIN 6
#endif
#ifndef NMSDK_PIXEL_LED_COUNT
#define NMSDK_PIXEL_LED_COUNT 8
#endif

#if NMSDK_PIXEL_HUB_WS2812
#include <Adafruit_NeoPixel.h>
Adafruit_NeoPixel strip(NMSDK_PIXEL_LED_COUNT, NMSDK_PIXEL_LED_PIN, NEO_GRB + NEO_KHZ800);
bool stripOk = false;
#endif
#if NMSDK_PIXEL_HUB_MAX7219
#include <LedControl.h>
LedControl lc = LedControl(11, 13, 10, 1); // DIN,CLK,CS
bool matrixOk = false;
#endif

float lastAck = 0;

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

void sendAck(float code)
{
  lastAck = code;
  float v[2] = {(float)NMSDK_PIXEL_LED_COUNT, lastAck};
  writeFloats(0x41, v, 2);
}

void sendPong()
{
  uint8_t one = 1;
  writeFramedV2(0x7F, &one, 1);
}

void setup()
{
  Serial.begin(NMSDK_PIXEL_HUB_BAUD);
#if NMSDK_PIXEL_HUB_WS2812
  strip.begin();
  strip.show();
  stripOk = true;
#endif
#if NMSDK_PIXEL_HUB_MAX7219
  lc.shutdown(0, false);
  lc.setIntensity(0, 8);
  lc.clearDisplay(0);
  matrixOk = true;
#endif
  sendAck(1);
}

void loop()
{
  if (Serial.available() <= 0)
    return;
  String command = Serial.readStringUntil('\n');
  command.trim();
  if (command == "PING") {
    sendPong();
  } else if (command.startsWith("PROTO ")) {
    Serial.println(F("PROTO OK 2"));
  } else if (command.startsWith("LED SET ")) {
#if NMSDK_PIXEL_HUB_WS2812
    // LED SET i r g b
    int p1 = command.indexOf(' ', 8);
    int p2 = command.indexOf(' ', p1 + 1);
    int p3 = command.indexOf(' ', p2 + 1);
    if (p1 > 0 && p2 > 0 && p3 > 0 && stripOk) {
      int i = command.substring(8, p1).toInt();
      int r = command.substring(p1 + 1, p2).toInt();
      int g = command.substring(p2 + 1, p3).toInt();
      int b = command.substring(p3 + 1).toInt();
      if (i >= 0 && i < (int)strip.numPixels())
        strip.setPixelColor(i, strip.Color(r, g, b));
    }
#endif
    sendAck(2);
  } else if (command == "LED FILL") {
#if NMSDK_PIXEL_HUB_WS2812
    if (stripOk)
      for (uint16_t i = 0; i < strip.numPixels(); ++i)
        strip.setPixelColor(i, strip.Color(32, 0, 0));
#endif
    sendAck(3);
  } else if (command == "LED SHOW") {
#if NMSDK_PIXEL_HUB_WS2812
    if (stripOk)
      strip.show();
#endif
    sendAck(4);
  } else if (command == "MATRIX CLEAR") {
#if NMSDK_PIXEL_HUB_MAX7219
    if (matrixOk)
      lc.clearDisplay(0);
#endif
    sendAck(5);
  } else if (command.startsWith("MATRIX TEXT ")) {
    String t = command.substring(12);
#if NMSDK_PIXEL_HUB_MAX7219
    if (matrixOk && t.length() > 0)
      lc.setChar(0, 0, t[0], false);
#else
    Serial.println(t);
#endif
    sendAck(6);
  } else if (command.startsWith("TFT FILL ")) {
    sendAck(7); // MVP: ack only unless TFT flag+lib wired
  } else if (command.startsWith("TFT TEXT ")) {
    Serial.println(command.substring(9));
    sendAck(8);
  } else if (command == "EPD CLEAR" || command.startsWith("EPD TEXT ")) {
#if NMSDK_PIXEL_HUB_EPD154
    // Waveshare lib optional; Serial stub keeps protocol honest when flag=0
#endif
    Serial.println(command);
    sendAck(9);
  }
}
