/*
 * nmsdk_radio_hub_v1 — NRF24 / LoRa / RC522 (+ optional PN532).
 * Host: nmsdk_radio_hub_v1. Frames: 0x50 RX, 0x51 UID, 0x7F pong.
 */
#include <Arduino.h>
#include <SPI.h>

#ifndef NMSDK_RADIO_HUB_BAUD
#define NMSDK_RADIO_HUB_BAUD 57600
#endif
#ifndef NMSDK_RADIO_HUB_NRF24
#define NMSDK_RADIO_HUB_NRF24 1
#endif
#ifndef NMSDK_RADIO_HUB_LORA
#define NMSDK_RADIO_HUB_LORA 0
#endif
#ifndef NMSDK_RADIO_HUB_RC522
#define NMSDK_RADIO_HUB_RC522 1
#endif
#ifndef NMSDK_RADIO_HUB_PN532
#define NMSDK_RADIO_HUB_PN532 0
#endif
#if NMSDK_RADIO_HUB_RC522 && NMSDK_RADIO_HUB_PN532
#undef NMSDK_RADIO_HUB_PN532
#define NMSDK_RADIO_HUB_PN532 0
#endif

#if NMSDK_RADIO_HUB_NRF24
#include <RF24.h>
RF24 radio(9, 10);
bool nrfOk = false;
#endif
#if NMSDK_RADIO_HUB_RC522
#include <MFRC522.h>
MFRC522 mfrc(10, 9);
bool rfidOk = false;
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

void sendPong()
{
  uint8_t one = 1;
  writeFramedV2(0x7F, &one, 1);
}

int hexNibble(char c)
{
  if (c >= '0' && c <= '9')
    return c - '0';
  if (c >= 'a' && c <= 'f')
    return c - 'a' + 10;
  if (c >= 'A' && c <= 'F')
    return c - 'A' + 10;
  return -1;
}

void setup()
{
  Serial.begin(NMSDK_RADIO_HUB_BAUD);
  SPI.begin();
#if NMSDK_RADIO_HUB_NRF24
  nrfOk = radio.begin();
  if (nrfOk) {
    radio.setPALevel(RF24_PA_LOW);
    const uint8_t addr[5] = {'N', 'M', 'S', 'D', 'K'};
    radio.openWritingPipe(addr);
    radio.openReadingPipe(1, addr);
    radio.startListening();
  }
#endif
#if NMSDK_RADIO_HUB_RC522
  mfrc.PCD_Init();
  rfidOk = true;
#endif
}

void loop()
{
#if NMSDK_RADIO_HUB_NRF24
  if (nrfOk && radio.available()) {
    uint8_t buf[32] = {0};
    radio.read(buf, sizeof(buf));
    float meta[2] = {(float)buf[0], 0};
    writeFloats(0x50, meta, 2);
  }
#endif
#if NMSDK_RADIO_HUB_RC522
  if (rfidOk && mfrc.PICC_IsNewCardPresent() && mfrc.PICC_ReadCardSerial()) {
    float uid = 0;
    for (byte i = 0; i < mfrc.uid.size && i < 4; ++i)
      uid = uid * 256.f + mfrc.uid.uidByte[i];
    writeFloats(0x51, &uid, 1);
    mfrc.PICC_HaltA();
  }
#endif

  if (Serial.available() <= 0)
    return;
  String command = Serial.readStringUntil('\n');
  command.trim();
  if (command == "PING") {
    sendPong();
  } else if (command.startsWith("PROTO ")) {
    Serial.println(F("PROTO OK 2"));
  } else if (command.startsWith("RADIO SEND ")) {
    String hex = command.substring(11);
    hex.replace(" ", "");
    uint8_t buf[32];
    int n = 0;
    for (int i = 0; i + 1 < (int)hex.length() && n < 32; i += 2) {
      int hi = hexNibble(hex[i]);
      int lo = hexNibble(hex[i + 1]);
      if (hi < 0 || lo < 0)
        break;
      buf[n++] = (uint8_t)((hi << 4) | lo);
    }
#if NMSDK_RADIO_HUB_NRF24
    if (nrfOk && n > 0) {
      radio.stopListening();
      radio.write(buf, n);
      radio.startListening();
    }
#else
    (void)n;
#endif
#if NMSDK_RADIO_HUB_LORA
    // LoRa driver optional behind flag
#endif
  }
}
