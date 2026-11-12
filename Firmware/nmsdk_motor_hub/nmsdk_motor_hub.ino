/*
 * nmsdk_motor_hub_v1 — DIR/PWM motor channels A+B (Motor Shield R3 defaults).
 * Host plugin: nmsdk_motor_hub_v1
 *
 * Commands:
 *   MOTOR A|B <pwm 0..255> | MOTOR A|B DIR <0|1> | MOTOR STOP | PING | PROTO 2
 *   SET PIN A|B dir|dir2|pwm|enable|brake|sense <Dn|An|NONE>
 *     (legacy: SET PIN dir … applies to A)
 *   WATCHDOG <ms>   (0 = off; auto MOTOR STOP if no host cmd within ms)
 *   GET PINS
 *
 * Frames:
 *   0x20 status: ch(u8) pwm dir sense_f32  — sent for A then B when reporting
 *   0x21 pin map: 12 bytes A(dir,dir2,pwm,enable,brake,sense), then B
 *     255 means that a pin role is not assigned.
 */
#include <Arduino.h>

#ifndef NMSDK_MOTOR_HUB_BAUD
#define NMSDK_MOTOR_HUB_BAUD 57600
#endif

#ifndef DIR_PIN_A
#define DIR_PIN_A 12
#endif
#ifndef PWM_PIN_A
#define PWM_PIN_A 3
#endif
#ifndef BRAKE_PIN_A
#define BRAKE_PIN_A 9
#endif
#ifndef SENSE_PIN_A
#define SENSE_PIN_A A0
#endif

#ifndef DIR_PIN_B
#define DIR_PIN_B 13
#endif
#ifndef PWM_PIN_B
#define PWM_PIN_B 11
#endif
#ifndef BRAKE_PIN_B
#define BRAKE_PIN_B 8
#endif
#ifndef SENSE_PIN_B
#define SENSE_PIN_B A1
#endif

uint8_t protocolVersion = 2;
bool reporting = true;
unsigned long mainDelay = 100;

struct MotorCh {
  uint8_t pwm;
  uint8_t dir;
  int dirPin;
  int dir2Pin;
  int pwmPin;
  int enablePin;
  int brakePin;
  int sensePin;
};

MotorCh chA = {0, 1, DIR_PIN_A, -1, PWM_PIN_A, -1, BRAKE_PIN_A, SENSE_PIN_A};
MotorCh chB = {0, 1, DIR_PIN_B, -1, PWM_PIN_B, -1, BRAKE_PIN_B, SENSE_PIN_B};

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

void applyMotor(MotorCh& m)
{
  if (m.dirPin >= 0)
    digitalWrite(m.dirPin, m.dir ? HIGH : LOW);
  if (m.dir2Pin >= 0)
    digitalWrite(m.dir2Pin, m.dir ? LOW : HIGH);
  if (m.brakePin >= 0)
    digitalWrite(m.brakePin, LOW);
  if (m.enablePin >= 0)
    digitalWrite(m.enablePin, HIGH);
  if (m.pwmPin >= 0)
    analogWrite(m.pwmPin, m.pwm);
}

void safeStopCh(MotorCh& m)
{
  m.pwm = 0;
  if (m.brakePin >= 0)
    digitalWrite(m.brakePin, HIGH);
  if (m.pwmPin >= 0)
    analogWrite(m.pwmPin, 0);
  if (m.enablePin >= 0)
    digitalWrite(m.enablePin, LOW);
}

void safeStop()
{
  safeStopCh(chA);
  safeStopCh(chB);
}

void touchHost()
{
  lastHostCmdMs = millis();
}

void sendMotorStatus(uint8_t channel, const MotorCh& m)
{
  float sense = m.sensePin >= 0 ? (float)analogRead(m.sensePin) : 0.0f;
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
  const auto pinValue = [](int pin) -> uint8_t { return pin < 0 ? 255 : (uint8_t)pin; };
  uint8_t body[12] = {
    pinValue(chA.dirPin), pinValue(chA.dir2Pin), pinValue(chA.pwmPin),
    pinValue(chA.enablePin), pinValue(chA.brakePin), pinValue(chA.sensePin),
    pinValue(chB.dirPin), pinValue(chB.dir2Pin), pinValue(chB.pwmPin),
    pinValue(chB.enablePin), pinValue(chB.brakePin), pinValue(chB.sensePin)
  };
  writeFramedV2(0x21, body, sizeof(body));
}

MotorCh* channelFromToken(const String& tok)
{
  if (tok == "A" || tok == "a")
    return &chA;
  if (tok == "B" || tok == "b")
    return &chB;
  return nullptr;
}

void setupPins(MotorCh& m)
{
  if (m.dirPin >= 0)
    pinMode(m.dirPin, OUTPUT);
  if (m.dir2Pin >= 0)
    pinMode(m.dir2Pin, OUTPUT);
  if (m.pwmPin >= 0)
    pinMode(m.pwmPin, OUTPUT);
  if (m.enablePin >= 0)
    pinMode(m.enablePin, OUTPUT);
  if (m.brakePin >= 0)
    pinMode(m.brakePin, OUTPUT);
  if (m.sensePin >= 0)
    pinMode(m.sensePin, INPUT);
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
      // MOTOR A DIR 1
      int sp1 = command.indexOf(' ', 6);
      if (sp1 > 0) {
        String chTok = command.substring(6, sp1);
        MotorCh* m = channelFromToken(chTok);
        int dirPos = command.indexOf("DIR");
        if (m && dirPos > 0) {
          m->dir = (uint8_t)command.substring(dirPos + 3).toInt() ? 1 : 0;
          applyMotor(*m);
        }
      }
    } else if (command.startsWith("MOTOR ")) {
      // MOTOR A 128
      int sp1 = command.indexOf(' ', 6);
      if (sp1 > 0) {
        String chTok = command.substring(6, sp1);
        MotorCh* m = channelFromToken(chTok);
        if (m) {
          int v = command.substring(sp1 + 1).toInt();
          if (v < 0)
            v = 0;
          if (v > 255)
            v = 255;
          m->pwm = (uint8_t)v;
          applyMotor(*m);
        }
      }
    } else if (command.startsWith("SET PIN ")) {
      // SET PIN A dir D12  |  SET PIN dir D12 (legacy → A)
      String rest = command.substring(8);
      rest.trim();
      MotorCh* m = &chA;
      int sp = rest.indexOf(' ');
      if (sp > 0) {
        String first = rest.substring(0, sp);
        if (first == "A" || first == "B" || first == "a" || first == "b") {
          m = channelFromToken(first);
          rest = rest.substring(sp + 1);
          rest.trim();
          sp = rest.indexOf(' ');
        }
      }
      if (m && sp > 0) {
        String role = rest.substring(0, sp);
        String pinStr = rest.substring(sp + 1);
        const bool clearPin = pinStr.equalsIgnoreCase("NONE") || pinStr == "-";
        const int pin = clearPin ? -1 : getPinFromString(pinStr);
        if (pin >= 0 || clearPin) {
          const int mode = role == "sense" ? INPUT : OUTPUT;
          if (role == "dir") {
            m->dirPin = pin;
          } else if (role == "dir2") {
            m->dir2Pin = pin;
          } else if (role == "pwm") {
            m->pwmPin = pin;
          } else if (role == "enable") {
            m->enablePin = pin;
          } else if (role == "brake") {
            m->brakePin = pin;
          } else if (role == "sense") {
            m->sensePin = pin;
          }
          if (pin >= 0)
            pinMode(pin, mode);
          applyMotor(*m);
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
