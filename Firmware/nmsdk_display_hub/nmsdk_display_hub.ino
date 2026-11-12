/*
 * nmsdk_display_hub_v1 — OLED SSD1306 / LCD 1602/2004 I2C text hub.
 * Host plugin: nmsdk_display_hub_v1
 * Frames: 0x7F pong; 0x40 status (rows, cols, driver_id)
 */
#include <Arduino.h>
#include <Wire.h>

#ifndef NMSDK_DISPLAY_HUB_BAUD
#define NMSDK_DISPLAY_HUB_BAUD 57600
#endif
#ifndef NMSDK_DISPLAY_HUB_SSD1306
#define NMSDK_DISPLAY_HUB_SSD1306 1
#endif
#ifndef NMSDK_DISPLAY_HUB_LCD1602
#define NMSDK_DISPLAY_HUB_LCD1602 1
#endif
#ifndef NMSDK_DISPLAY_HUB_LCD2004
#define NMSDK_DISPLAY_HUB_LCD2004 0
#endif

#if NMSDK_DISPLAY_HUB_SSD1306
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
Adafruit_SSD1306 oled(128, 64, &Wire, -1);
bool oledOk = false;
#endif
#if NMSDK_DISPLAY_HUB_LCD1602 || NMSDK_DISPLAY_HUB_LCD2004
#include <LiquidCrystal_I2C.h>
#if NMSDK_DISPLAY_HUB_LCD2004
LiquidCrystal_I2C lcd(0x27, 20, 4);
const float kRows = 4, kCols = 20, kDriver = 2;
#else
LiquidCrystal_I2C lcd(0x27, 16, 2);
const float kRows = 2, kCols = 16, kDriver = 1;
#endif
bool lcdOk = false;
#else
const float kRows = 0, kCols = 0, kDriver = 0;
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

void sendStatus()
{
  float v[3] = {kRows, kCols, kDriver};
#if NMSDK_DISPLAY_HUB_SSD1306
  if (oledOk) {
    v[0] = 8;
    v[1] = 21;
    v[2] = 3;
  }
#endif
  writeFloats(0x40, v, 3);
}

void sendPong()
{
  uint8_t one = 1;
  writeFramedV2(0x7F, &one, 1);
}

void setup()
{
  Serial.begin(NMSDK_DISPLAY_HUB_BAUD);
  Wire.begin();
#if NMSDK_DISPLAY_HUB_SSD1306
  oledOk = oled.begin(SSD1306_SWITCHCAPVCC, 0x3C);
  if (oledOk) {
    oled.clearDisplay();
    oled.display();
  }
#endif
#if NMSDK_DISPLAY_HUB_LCD1602 || NMSDK_DISPLAY_HUB_LCD2004
  lcd.init();
  lcd.backlight();
  lcdOk = true;
#endif
  sendStatus();
}

void loop()
{
  if (Serial.available() <= 0)
    return;
  String command = Serial.readStringUntil('\n');
  command.trim();
  if (command == "PING" || command == "GET STATUS") {
    sendPong();
    sendStatus();
  } else if (command.startsWith("PROTO ")) {
    Serial.println(F("PROTO OK 2"));
  } else if (command == "CLEAR") {
#if NMSDK_DISPLAY_HUB_LCD1602 || NMSDK_DISPLAY_HUB_LCD2004
    if (lcdOk)
      lcd.clear();
#endif
  } else if (command.startsWith("PRINT ")) {
    // PRINT <row> <col> <text>
    int sp1 = command.indexOf(' ', 6);
    int sp2 = command.indexOf(' ', sp1 + 1);
    if (sp1 > 0 && sp2 > 0) {
      int row = command.substring(6, sp1).toInt();
      int col = command.substring(sp1 + 1, sp2).toInt();
      String text = command.substring(sp2 + 1);
#if NMSDK_DISPLAY_HUB_LCD1602 || NMSDK_DISPLAY_HUB_LCD2004
      if (lcdOk) {
        lcd.setCursor(col, row);
        lcd.print(text);
      }
#else
      (void)row;
      (void)col;
      Serial.println(text);
#endif
    }
  } else if (command == "OLED CLEAR") {
#if NMSDK_DISPLAY_HUB_SSD1306
    if (oledOk) {
      oled.clearDisplay();
      oled.display();
    }
#endif
  } else if (command.startsWith("OLED PRINT ")) {
    String text = command.substring(11);
#if NMSDK_DISPLAY_HUB_SSD1306
    if (oledOk) {
      oled.clearDisplay();
      oled.setTextSize(1);
      oled.setTextColor(SSD1306_WHITE);
      oled.setCursor(0, 0);
      oled.println(text);
      oled.display();
    }
#else
    Serial.println(text);
#endif
  }
}
