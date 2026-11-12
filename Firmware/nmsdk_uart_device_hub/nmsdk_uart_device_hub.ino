/*
 * nmsdk_uart_device_hub_v1 — Nextion / HC-05 / SIM800 / A9G on second UART.
 * Host USB = Serial; device = Serial1 (Mega/ESP) or SoftwareSerial 10/11 (Uno).
 * Frame 0x60 = UTF-8 line bytes from device.
 *
 * Flags select which command families are accepted (compile-time product profile).
 */
#include <Arduino.h>

#ifndef NMSDK_UART_HUB_BAUD
#define NMSDK_UART_HUB_BAUD 57600
#endif
#ifndef NMSDK_UART_DEVICE_BAUD
#define NMSDK_UART_DEVICE_BAUD 9600
#endif
#ifndef NMSDK_UART_HUB_NEXTION
#define NMSDK_UART_HUB_NEXTION 1
#endif
#ifndef NMSDK_UART_HUB_HC05
#define NMSDK_UART_HUB_HC05 1
#endif
#ifndef NMSDK_UART_HUB_SIM800
#define NMSDK_UART_HUB_SIM800 0
#endif
#ifndef NMSDK_UART_HUB_A9G
#define NMSDK_UART_HUB_A9G 0
#endif

#if defined(ARDUINO_AVR_MEGA2560) || defined(ESP32)
#define NMSDK_HAS_HW_SERIAL1 1
#else
#define NMSDK_HAS_HW_SERIAL1 0
#include <SoftwareSerial.h>
SoftwareSerial deviceSerial(10, 11);
#endif

bool bridgeMode = false;

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
  uint8_t frame[4 + 96];
  memcpy(frame, header, 4);
  int fl = 4;
  if (len && len <= 92) {
    memcpy(frame + 4, payload, len);
    fl += len;
  }
  Serial.write(crc8Maxim(frame, fl));
}

Stream& device()
{
#if NMSDK_HAS_HW_SERIAL1
  return Serial1;
#else
  return deviceSerial;
#endif
}

void sendLineFrame(const String& line)
{
  writeFramedV2(0x60, (const uint8_t*)line.c_str(), (uint16_t)line.length());
}

bool allowAtFamily()
{
#if NMSDK_UART_HUB_HC05 || NMSDK_UART_HUB_SIM800 || NMSDK_UART_HUB_A9G
  return true;
#else
  return false;
#endif
}

void setup()
{
  Serial.begin(NMSDK_UART_HUB_BAUD);
#if NMSDK_HAS_HW_SERIAL1
  Serial1.begin(NMSDK_UART_DEVICE_BAUD);
#else
  deviceSerial.begin(NMSDK_UART_DEVICE_BAUD);
#endif
}

void loop()
{
  if (device().available() > 0) {
    String line = device().readStringUntil('\n');
    line.trim();
    if (line.length())
      sendLineFrame(line);
  }

  if (Serial.available() <= 0)
    return;
  String command = Serial.readStringUntil('\n');
  command.trim();
  if (command == "PING") {
    uint8_t one = 1;
    writeFramedV2(0x7F, &one, 1);
  } else if (command.startsWith("PROTO ")) {
    Serial.println(F("PROTO OK 2"));
  } else if (command == "BRIDGE ON") {
    bridgeMode = true;
  } else if (command == "BRIDGE OFF") {
    bridgeMode = false;
  } else if (command.startsWith("HMI TX ")) {
#if NMSDK_UART_HUB_NEXTION
    String payload = command.substring(7);
    device().print(payload);
    device().write(0xFF);
    device().write(0xFF);
    device().write(0xFF);
#else
    Serial.println(F("ERR NEXTION disabled"));
#endif
  } else if (command.startsWith("AT")) {
    if (!allowAtFamily()) {
      Serial.println(F("ERR AT family disabled"));
      return;
    }
#if NMSDK_UART_HUB_A9G
    // A9G GPS peek: host may send AT+GPSRD; forward as-is
#endif
#if NMSDK_UART_HUB_SIM800 || NMSDK_UART_HUB_A9G || NMSDK_UART_HUB_HC05
    device().println(command);
#endif
  } else if (bridgeMode) {
    device().println(command);
  }
}
