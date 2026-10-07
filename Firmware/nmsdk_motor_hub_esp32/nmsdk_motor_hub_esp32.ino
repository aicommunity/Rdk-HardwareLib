/*
 * nmsdk_motor_hub_esp32_v1 — same wire protocol as AVR motor hub (A+B).
 * Baud default 115200. Flash with esptool / Arduino ESP32 core.
 */
#include <Arduino.h>

#ifndef NMSDK_MOTOR_HUB_BAUD
#define NMSDK_MOTOR_HUB_BAUD 115200
#endif

// ESP32 Uno-form-factor / shield-friendly defaults (adjust per board)
#ifndef DIR_PIN_A
#define DIR_PIN_A 12
#endif
#ifndef PWM_PIN_A
#define PWM_PIN_A 25
#endif
#ifndef BRAKE_PIN_A
#define BRAKE_PIN_A 26
#endif
#ifndef SENSE_PIN_A
#define SENSE_PIN_A 34
#endif
#ifndef DIR_PIN_B
#define DIR_PIN_B 13
#endif
#ifndef PWM_PIN_B
#define PWM_PIN_B 27
#endif
#ifndef BRAKE_PIN_B
#define BRAKE_PIN_B 14
#endif
#ifndef SENSE_PIN_B
#define SENSE_PIN_B 35
#endif

uint8_t protocolVersion = 2;
bool reporting = true;
unsigned long mainDelay = 100;

struct MotorCh {
  uint8_t pwm;
  uint8_t dir;
  int dirPin;
  int pwmPin;
  int brakePin;
  int sensePin;
  int ledcChannel;
};

MotorCh chA = {0, 1, DIR_PIN_A, PWM_PIN_A, BRAKE_PIN_A, SENSE_PIN_A, 0};
MotorCh chB = {0, 1, DIR_PIN_B, PWM_PIN_B, BRAKE_PIN_B, SENSE_PIN_B, 1};

unsigned long watchdogMs = 2000;
unsigned long lastHostCmdMs = 0;

int getPinFromString(String pinStr)
{
  pinStr.trim();
  pinStr.toUpperCase();
  if (pinStr.startsWith("A") && pinStr.length() >= 2)
    return pinStr.substring(1).toInt();
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

void applyMotor(MotorCh& m)
{
  digitalWrite(m.dirPin, m.dir ? HIGH : LOW);
  digitalWrite(m.brakePin, LOW);
  ledcWrite(m.ledcChannel, m.pwm);
}

void safeStopCh(MotorCh& m)
{
  m.pwm = 0;
  digitalWrite(m.brakePin, HIGH);
  ledcWrite(m.ledcChannel, 0);
}

void safeStop()
{
  safeStopCh(chA);
  safeStopCh(chB);
}

void touchHost() { lastHostCmdMs = millis(); }

void sendMotorStatus(uint8_t channel, const MotorCh& m)
{
  float sense = (float)analogRead(m.sensePin);
  uint8_t body[7];
  body[0] = channel;
  body[1] = m.pwm;
  body[2] = m.dir;
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
  uint8_t body[8] = {
      (uint8_t)chA.dirPin, (uint8_t)chA.pwmPin, (uint8_t)chA.brakePin, (uint8_t)chA.sensePin,
      (uint8_t)chB.dirPin, (uint8_t)chB.pwmPin, (uint8_t)chB.brakePin, (uint8_t)chB.sensePin};
  writeFramedV2(0x21, body, 8);
}

MotorCh* channelFromToken(const String& tok)
{
  if (tok == "A" || tok == "a") return &chA;
  if (tok == "B" || tok == "b") return &chB;
  return nullptr;
}

void setupPins(MotorCh& m)
{
  pinMode(m.dirPin, OUTPUT);
  pinMode(m.brakePin, OUTPUT);
  pinMode(m.sensePin, INPUT);
  ledcSetup(m.ledcChannel, 5000, 8);
  ledcAttachPin(m.pwmPin, m.ledcChannel);
}

void setup()
{
  Serial.begin(NMSDK_MOTOR_HUB_BAUD);
  setupPins(chA);
  setupPins(chB);
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
    } else if (command.startsWith("MOTOR ") && command.indexOf(" DIR") > 0) {
      int sp1 = command.indexOf(' ', 6);
      if (sp1 > 0) {
        MotorCh* m = channelFromToken(command.substring(6, sp1));
        int dirPos = command.indexOf("DIR");
        if (m && dirPos > 0) {
          m->dir = (uint8_t)command.substring(dirPos + 3).toInt() ? 1 : 0;
          applyMotor(*m);
        }
      }
    } else if (command.startsWith("MOTOR ")) {
      int sp1 = command.indexOf(' ', 6);
      if (sp1 > 0) {
        MotorCh* m = channelFromToken(command.substring(6, sp1));
        if (m) {
          int v = constrain(command.substring(sp1 + 1).toInt(), 0, 255);
          m->pwm = (uint8_t)v;
          applyMotor(*m);
        }
      }
    } else if (command.startsWith("WATCHDOG ")) {
      long ms = command.substring(9).toInt();
      if (ms < 0) ms = 0;
      watchdogMs = (unsigned long)ms;
    } else if (command == "GET PINS") {
      sendPinConfig();
    } else if (command == "PING" || command == "GET STATUS") {
      sendPong();
      sendMotorStatus(0, chA);
      sendMotorStatus(1, chB);
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

  if (watchdogMs > 0 && (chA.pwm > 0 || chB.pwm > 0) && (millis() - lastHostCmdMs) > watchdogMs)
    safeStop();

  if (reporting) {
    sendMotorStatus(0, chA);
    sendMotorStatus(1, chB);
  }
  delay(mainDelay);
}
