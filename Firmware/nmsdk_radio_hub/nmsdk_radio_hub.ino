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
#ifndef NMSDK_NRF24_CE
#define NMSDK_NRF24_CE 9
#endif
#ifndef NMSDK_NRF24_CSN
#define NMSDK_NRF24_CSN 10
#endif
RF24 radio(NMSDK_NRF24_CE, NMSDK_NRF24_CSN);
bool nrfOk = false;
#endif
#if NMSDK_RADIO_HUB_LORA
#include <LoRa.h>
bool loraOk = false;
#ifndef NMSDK_LORA_SS
#define NMSDK_LORA_SS 10
#endif
#ifndef NMSDK_LORA_RST
#define NMSDK_LORA_RST 9
#endif
#ifndef NMSDK_LORA_DIO0
#define NMSDK_LORA_DIO0 2
#endif
#endif
#if NMSDK_RADIO_HUB_RC522
#include <MFRC522.h>
#ifndef NMSDK_RC522_SS
#define NMSDK_RC522_SS 8
#endif
#ifndef NMSDK_RC522_RST
#define NMSDK_RC522_RST 7
#endif
MFRC522 mfrc(NMSDK_RC522_SS, NMSDK_RC522_RST);
bool rfidOk = false;
#endif
#if NMSDK_RADIO_HUB_PN532
#include <Adafruit_PN532.h>
Adafruit_PN532 nfc(2, 3); // IRQ, RESET (I2C)
bool pn532Ok = false;
#endif

#if NMSDK_RADIO_HUB_NRF24 && NMSDK_RADIO_HUB_RC522 \
    && NMSDK_NRF24_CSN == NMSDK_RC522_SS
#error "NRF24 CSN and RC522 SS must use different pins"
#endif
#if NMSDK_RADIO_HUB_NRF24 && NMSDK_RADIO_HUB_LORA \
    && NMSDK_NRF24_CSN == NMSDK_LORA_SS
#error "NRF24 CSN and LoRa SS must use different pins"
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
#if NMSDK_RADIO_HUB_LORA
  LoRa.setPins(NMSDK_LORA_SS, NMSDK_LORA_RST, NMSDK_LORA_DIO0);
  loraOk = LoRa.begin(433E6);
#endif
#if NMSDK_RADIO_HUB_RC522
  mfrc.PCD_Init();
  rfidOk = true;
#endif
#if NMSDK_RADIO_HUB_PN532
  nfc.begin();
  uint32_t ver = nfc.getFirmwareVersion();
  pn532Ok = ver != 0;
  if (pn532Ok)
    nfc.SAMConfig();
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
#if NMSDK_RADIO_HUB_LORA
  if (loraOk) {
    int packetSize = LoRa.parsePacket();
    if (packetSize > 0) {
      float meta[2] = {(float)packetSize, (float)LoRa.packetRssi()};
      writeFloats(0x50, meta, 2);
      while (LoRa.available())
        (void)LoRa.read();
    }
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
#if NMSDK_RADIO_HUB_PN532
  if (pn532Ok) {
    uint8_t uidbuf[7] = {0};
    uint8_t uidLen = 0;
    if (nfc.readPassiveTargetID(PN532_MIFARE_ISO14443A, uidbuf, &uidLen)) {
      float uid = 0;
      for (uint8_t i = 0; i < uidLen && i < 4; ++i)
        uid = uid * 256.f + uidbuf[i];
      writeFloats(0x51, &uid, 1);
    }
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
#endif
#if NMSDK_RADIO_HUB_LORA
    if (loraOk && n > 0) {
      LoRa.beginPacket();
      LoRa.write(buf, n);
      LoRa.endPacket();
    }
#endif
#if !NMSDK_RADIO_HUB_NRF24 && !NMSDK_RADIO_HUB_LORA
    (void)n;
#endif
  }
}
