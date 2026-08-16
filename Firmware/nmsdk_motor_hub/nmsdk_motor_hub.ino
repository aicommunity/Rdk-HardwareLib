/*
 * nmsdk_motor_hub_v1 — DIR/PWM motor channel A (Motor Shield R3 defaults).
 * Host plugin: nmsdk_motor_hub_v1
 *
 * Commands:
 *   MOTOR A <pwm 0..255> | MOTOR A DIR <0|1> | MOTOR STOP | PING | PROTO 2
 *   SET PIN dir|pwm|brake|sense <Dn|An>
 *   WATCHDOG <ms>   (0 = off; auto MOTOR STOP if no host cmd within ms)
 *   GET PINS
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

int dirPin = DIR_PIN;
int pwmPin = PWM_PIN;
int brakePin = BRAKE_PIN;
int sensePin = SENSE_PIN;

unsigned long watchdogMs = 2000;
unsigned long lastHostCmdMs = 0;

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

void applyMotor()
{
  digitalWrite(dirPin, motorDir ? HIGH : LOW);
  digitalWrite(brakePin, LOW);
  analogWrite(pwmPin, motorPwm);
}

void safeStop()
{
  motorPwm = 0;
  digitalWrite(brakePin, HIGH);
  analogWrite(pwmPin, 0);
}

void touchHost()
{
  lastHostCmdMs = millis();
}

void sendMotorStatus()
{
  float sense = (float)analogRead(sensePin);
  uint8_t body[7];
  body[0] = 0;
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

void sendPinConfig()
{
  // type 0x04-like compact: dir,pwm,brake,sense as uint8 firmata-ish pins (0..63)
  uint8_t body[4];
  body[0] = (uint8_t)dirPin;
  body[1] = (uint8_t)pwmPin;
  body[2] = (uint8_t)brakePin;
  body[3] = (uint8_t)sensePin;
  writeFramedV2(0x21, body, 4);
  Serial.print(F("PINS dir="));
  Serial.print(dirPin);
  Serial.print(F(" pwm="));
  Serial.print(pwmPin);
  Serial.print(F(" brake="));
  Serial.print(brakePin);
  Serial.print(F(" sense="));
  Serial.println(sensePin);
}

void setup()
{
  Serial.begin(NMSDK_MOTOR_HUB_BAUD);
  pinMode(dirPin, OUTPUT);
  pinMode(pwmPin, OUTPUT);
  pinMode(brakePin, OUTPUT);
  pinMode(sensePin, INPUT);
  safeStop();
  touchHost();
}

void loop()
{
  if (Serial.available() > 0) {
    String command = Serial.readStringUntil('\n');
    command.trim();
    touchHost();
    if (command == "MOTOR STOP" || command == "STOP") {
      safeStop();
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
    } else if (command.startsWith("SET PIN ")) {
      // SET PIN dir D12
      int sp = command.indexOf(' ', 8);
      if (sp > 0) {
        String role = command.substring(8, sp);
        String pinStr = command.substring(sp + 1);
        int pin = getPinFromString(pinStr);
        if (pin >= 0) {
          if (role == "dir") {
            dirPin = pin;
            pinMode(dirPin, OUTPUT);
          } else if (role == "pwm") {
            pwmPin = pin;
            pinMode(pwmPin, OUTPUT);
          } else if (role == "brake") {
            brakePin = pin;
            pinMode(brakePin, OUTPUT);
          } else if (role == "sense") {
            sensePin = pin;
            pinMode(sensePin, INPUT);
          }
          applyMotor();
          sendPinConfig();
        }
      }
    } else if (command.startsWith("WATCHDOG ")) {
      long ms = command.substring(9).toInt();
      if (ms < 0)
        ms = 0;
      watchdogMs = (unsigned long)ms;
    } else if (command == "GET PINS") {
      sendPinConfig();
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

  if (watchdogMs > 0 && motorPwm > 0 && (millis() - lastHostCmdMs) > watchdogMs)
    safeStop();

  if (reporting)
    sendMotorStatus();
  delay(mainDelay);
}
