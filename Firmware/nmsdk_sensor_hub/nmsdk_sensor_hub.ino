/*
 * nmsdk_sensor_hub_v1 — DHT11 + HC-SR04 + analog on Sensor Shield (framed v2).
 * Host plugin: nmsdk_sensor_hub_v1
 *
 * Commands:
 *   START/STOP READING | PING | GET STATUS | PROTO 2 | SET DELAY <ms>
 *   SET DEVICE dht|trig|echo|hall <Dn|An>
 *   CLEAR DEVICES   (restore compile-time defaults)
 *   GET PINS
 */
#include <Arduino.h>
#include <DHT.h>

#ifndef NMSDK_SENSOR_HUB_BAUD
#define NMSDK_SENSOR_HUB_BAUD 57600
#endif

#ifndef DHTPIN
#define DHTPIN 2
#endif
#ifndef DHTTYPE
#define DHTTYPE DHT11
#endif
#ifndef TRIGPIN
#define TRIGPIN 8
#endif
#ifndef ECHOPIN
#define ECHOPIN 7
#endif
#ifndef HALLPIN
#define HALLPIN A2
#endif

DHT dht(DHTPIN, DHTTYPE);
bool readingEnabled = true;
uint8_t protocolVersion = 2;
unsigned long mainDelay = 500;

int dhtPin = DHTPIN;
int trigPin = TRIGPIN;
int echoPin = ECHOPIN;
int hallPin = HALLPIN;

int getPinFromString(String pinStr)
{
  pinStr.trim();
  pinStr.toUpperCase();
  if (pinStr.startsWith("A") && pinStr.length() >= 2) {
    int ch = pinStr.substring(1).toInt();
    return A0 + ch;
  }
  if (pinStr.startsWith("D") && pinStr.length() >= 2)
    return pinStr.substring(1).toInt();
  if (pinStr.length() > 0 && isDigit(pinStr.charAt(0)))
    return pinStr.toInt();
  return -1;
}

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

float readDistanceCm()
{
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);
  const unsigned long us = pulseIn(echoPin, HIGH, 25000UL);
  if (us == 0)
    return -1.f;
  return us / 58.0f;
}

void sendSensorData()
{
  float t = dht.readTemperature();
  float h = dht.readHumidity();
  float dist = readDistanceCm();
  float hall = (float)analogRead(hallPin);
  if (isnan(t))
    t = 0;
  if (isnan(h))
    h = 0;
  uint8_t body[2 + 4 * sizeof(float)];
  body[0] = 0;
  body[1] = 4;
  memcpy(body + 2, &t, 4);
  memcpy(body + 6, &h, 4);
  memcpy(body + 10, &dist, 4);
  memcpy(body + 14, &hall, 4);
  writeFramedV2(0x01, body, sizeof(body));
}

void sendPong()
{
  uint8_t one = 1;
  writeFramedV2(0x7F, &one, 1);
}

void sendPinConfig()
{
  uint8_t body[4];
  body[0] = (uint8_t)dhtPin;
  body[1] = (uint8_t)trigPin;
  body[2] = (uint8_t)echoPin;
  body[3] = (uint8_t)hallPin;
  writeFramedV2(0x22, body, 4);
  Serial.print(F("PINS dht="));
  Serial.print(dhtPin);
  Serial.print(F(" trig="));
  Serial.print(trigPin);
  Serial.print(F(" echo="));
  Serial.print(echoPin);
  Serial.print(F(" hall="));
  Serial.println(hallPin);
}

void rebindDht(int pin)
{
  dhtPin = pin;
  dht = DHT(dhtPin, DHTTYPE);
  dht.begin();
}

void setup()
{
  Serial.begin(NMSDK_SENSOR_HUB_BAUD);
  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);
  pinMode(hallPin, INPUT);
  dht.begin();
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
    else if (command == "GET PINS")
      sendPinConfig();
    else if (command == "CLEAR DEVICES") {
      rebindDht(DHTPIN);
      trigPin = TRIGPIN;
      echoPin = ECHOPIN;
      hallPin = HALLPIN;
      pinMode(trigPin, OUTPUT);
      pinMode(echoPin, INPUT);
      pinMode(hallPin, INPUT);
      sendPinConfig();
    } else if (command.startsWith("SET DEVICE ")) {
      // SET DEVICE dht D2
      int sp = command.indexOf(' ', 11);
      if (sp > 0) {
        String role = command.substring(11, sp);
        String pinStr = command.substring(sp + 1);
        int pin = getPinFromString(pinStr);
        if (pin >= 0) {
          if (role == "dht")
            rebindDht(pin);
          else if (role == "trig") {
            trigPin = pin;
            pinMode(trigPin, OUTPUT);
          } else if (role == "echo") {
            echoPin = pin;
            pinMode(echoPin, INPUT);
          } else if (role == "hall") {
            hallPin = pin;
            pinMode(hallPin, INPUT);
          }
          sendPinConfig();
        }
      }
    } else if (command.startsWith("PROTO ")) {
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
