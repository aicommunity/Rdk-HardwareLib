/*
 * nmsdk_motor_hub_v1 — DIR/PWM motor channel A (Motor Shield R3 defaults).
 * Host plugin: nmsdk_motor_hub_v1
 * Commands: MOTOR A <pwm 0..255>, MOTOR A DIR <0|1>, MOTOR STOP, PING, PROTO 2
 */
#include <Arduino.h>

#ifndef NMSDK_MOTOR_HUB_BAUD
#define NMSDK_MOTOR_HUB_BAUD 57600
#endif

#ifndef DIR_PIN
#define DIR_PIN 12
#endif
#ifndef PWM_PIN
#define PWM_PIN 3
#endif
#ifndef BRAKE_PIN
#define BRAKE_PIN 9
#endif
#ifndef SENSE_PIN
#define SENSE_PIN A0
#endif

uint8_t protocolVersion = 2;
uint8_t motorPwm = 0;
uint8_t motorDir = 1;
bool reporting = true;
unsigned long mainDelay = 100;

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

void applyMotor()
{
  digitalWrite(DIR_PIN, motorDir ? HIGH : LOW);
  digitalWrite(BRAKE_PIN, LOW);
  analogWrite(PWM_PIN, motorPwm);
}

void sendMotorStatus()
{
  float sense = (float)analogRead(SENSE_PIN);
  uint8_t body[7];
  body[0] = 0; // channel A
  body[1] = motorPwm;
  body[2] = motorDir;
  memcpy(body + 3, &sense, 4);
  writeFramedV2(0x20, body, sizeof(body));
}

void sendPong()
{
  uint8_t one = 1;
  writeFramedV2(0x7F, &one, 1);
}

void setup()
{
  Serial.begin(NMSDK_MOTOR_HUB_BAUD);
  pinMode(DIR_PIN, OUTPUT);
  pinMode(PWM_PIN, OUTPUT);
  pinMode(BRAKE_PIN, OUTPUT);
  pinMode(SENSE_PIN, INPUT);
  applyMotor();
}

void loop()
{
  if (Serial.available() > 0) {
    String command = Serial.readStringUntil('\n');
    command.trim();
    if (command == "MOTOR STOP" || command == "STOP") {
      motorPwm = 0;
      applyMotor();
    } else if (command.startsWith("MOTOR A DIR")) {
      motorDir = (uint8_t)command.substring(12).toInt() ? 1 : 0;
      applyMotor();
    } else if (command.startsWith("MOTOR A ")) {
      int v = command.substring(8).toInt();
      if (v < 0)
        v = 0;
      if (v > 255)
        v = 255;
      motorPwm = (uint8_t)v;
      applyMotor();
    } else if (command == "PING" || command == "GET STATUS") {
      sendPong();
      sendMotorStatus();
    } else if (command == "START READING") {
      reporting = true;
    } else if (command == "STOP READING") {
      reporting = false;
    } else if (command.startsWith("PROTO ")) {
      int ver = command.substring(6).toInt();
      protocolVersion = (ver >= 2) ? 2 : 1;
      Serial.println(ver >= 2 ? F("PROTO OK 2") : F("PROTO OK 1"));
    }
  }

  if (reporting)
    sendMotorStatus();
  delay(mainDelay);
}
