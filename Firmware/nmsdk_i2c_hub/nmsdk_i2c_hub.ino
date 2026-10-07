/*
 * nmsdk_i2c_hub_v1 — framed v2 I2C hub (P2 Tier C).
 * Host plugin: nmsdk_i2c_hub_v1
 *
 * Frame types (payload = [0,n] + n×float LE unless noted):
 *   0x01 climate  — t_c, humidity_pct, pressure_hpa   (BME280)
 *   0x30 ToF      — distance_mm                       (VL53L0X)
 *   0x31 IMU      — ax,ay,az,gx,gy,gz                   (MPU6050)
 *   0x32 power    — bus_v, current_ma, power_mw       (INA219)
 *   0x33 PCA echo — ch, duty_12bit                    (after SET PWM)
 *
 * Commands:
 *   PROTO 2 | PING | START/STOP READING | SET DELAY <ms>
 *   SET PWM <ch0-15> <0-4095>
 *
 * Build flags (1=enable, default as below):
 *   -DNMSDK_I2C_HUB_BME280=1
 *   -DNMSDK_I2C_HUB_VL53=1
 *   -DNMSDK_I2C_HUB_MPU6050=1
 *   -DNMSDK_I2C_HUB_INA219=1
 *   -DNMSDK_I2C_HUB_PCA9685=1
 */
#include <Arduino.h>
#include <Wire.h>

#ifndef NMSDK_I2C_HUB_BAUD
#define NMSDK_I2C_HUB_BAUD 57600
#endif
#ifndef NMSDK_I2C_HUB_BME280
#define NMSDK_I2C_HUB_BME280 1
#endif
#ifndef NMSDK_I2C_HUB_VL53
#define NMSDK_I2C_HUB_VL53 1
#endif
#ifndef NMSDK_I2C_HUB_MPU6050
#define NMSDK_I2C_HUB_MPU6050 1
#endif
#ifndef NMSDK_I2C_HUB_INA219
#define NMSDK_I2C_HUB_INA219 1
#endif
#ifndef NMSDK_I2C_HUB_PCA9685
#define NMSDK_I2C_HUB_PCA9685 1
#endif

#if NMSDK_I2C_HUB_BME280
#include <Adafruit_BME280.h>
Adafruit_BME280 bme;
bool bmeOk = false;
#endif
#if NMSDK_I2C_HUB_VL53
#include <Adafruit_VL53L0X.h>
Adafruit_VL53L0X lox;
bool vl53Ok = false;
#endif
#if NMSDK_I2C_HUB_MPU6050
#include <Adafruit_MPU6050.h>
#include <Adafruit_Sensor.h>
Adafruit_MPU6050 mpu;
bool mpuOk = false;
#endif
#if NMSDK_I2C_HUB_INA219
#include <Adafruit_INA219.h>
Adafruit_INA219 ina219;
bool inaOk = false;
#endif
#if NMSDK_I2C_HUB_PCA9685
#include <Adafruit_PWMServoDriver.h>
Adafruit_PWMServoDriver pca = Adafruit_PWMServoDriver();
bool pcaOk = false;
#endif

bool readingEnabled = true;
uint8_t protocolVersion = 2;
unsigned long mainDelay = 500;

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

void sendClimate()
{
  float v[3] = {0, 0, 0};
#if NMSDK_I2C_HUB_BME280
  if (bmeOk) {
    v[0] = bme.readTemperature();
    v[1] = bme.readHumidity();
    v[2] = bme.readPressure() / 100.0f;
  }
#endif
  writeFloats(0x01, v, 3);
}

void sendTof()
{
#if NMSDK_I2C_HUB_VL53
  float v[1] = {0};
  if (vl53Ok) {
    VL53L0X_RangingMeasurementData_t m;
    lox.rangingTest(&m, false);
    v[0] = (m.RangeStatus != 4) ? (float)m.RangeMilliMeter : -1.f;
  }
  writeFloats(0x30, v, 1);
#endif
}

void sendImu()
{
#if NMSDK_I2C_HUB_MPU6050
  float v[6] = {0, 0, 0, 0, 0, 0};
  if (mpuOk) {
    sensors_event_t a, g, temp;
    mpu.getEvent(&a, &g, &temp);
    v[0] = a.acceleration.x;
    v[1] = a.acceleration.y;
    v[2] = a.acceleration.z;
    v[3] = g.gyro.x;
    v[4] = g.gyro.y;
    v[5] = g.gyro.z;
  }
  writeFloats(0x31, v, 6);
#endif
}

void sendPower()
{
#if NMSDK_I2C_HUB_INA219
  float v[3] = {0, 0, 0};
  if (inaOk) {
    v[0] = ina219.getBusVoltage_V();
    v[1] = ina219.getCurrent_mA();
    v[2] = ina219.getPower_mW();
  }
  writeFloats(0x32, v, 3);
#endif
}

void sendAllSensors()
{
  sendClimate();
  sendTof();
  sendImu();
  sendPower();
}

void sendPong()
{
  uint8_t one = 1;
  writeFramedV2(0x7F, &one, 1);
}

void setup()
{
  Serial.begin(NMSDK_I2C_HUB_BAUD);
  Wire.begin();
#if NMSDK_I2C_HUB_BME280
  bmeOk = bme.begin(0x76) || bme.begin(0x77);
  if (!bmeOk)
    Serial.println(F("BME280 not found"));
#endif
#if NMSDK_I2C_HUB_VL53
  vl53Ok = lox.begin();
  if (!vl53Ok)
    Serial.println(F("VL53L0X not found"));
#endif
#if NMSDK_I2C_HUB_MPU6050
  mpuOk = mpu.begin();
  if (mpuOk) {
    mpu.setAccelerometerRange(MPU6050_RANGE_8_G);
    mpu.setGyroRange(MPU6050_RANGE_500_DEG);
  } else
    Serial.println(F("MPU6050 not found"));
#endif
#if NMSDK_I2C_HUB_INA219
  inaOk = ina219.begin();
  if (!inaOk)
    Serial.println(F("INA219 not found"));
#endif
#if NMSDK_I2C_HUB_PCA9685
  pca.begin();
  pca.setPWMFreq(50);
  pcaOk = true;
#endif
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
    else if (command.startsWith("PROTO ")) {
      int ver = command.substring(6).toInt();
      protocolVersion = (ver >= 2) ? 2 : 1;
      Serial.println(ver >= 2 ? F("PROTO OK 2") : F("PROTO OK 1"));
    } else if (command.startsWith("SET DELAY")) {
      int d = command.substring(10).toInt();
      if (d > 0)
        mainDelay = d;
    } else if (command.startsWith("SET PWM ")) {
#if NMSDK_I2C_HUB_PCA9685
      // SET PWM <ch> <duty>
      int sp = command.indexOf(' ', 8);
      if (sp > 0 && pcaOk) {
        int ch = command.substring(8, sp).toInt();
        int duty = command.substring(sp + 1).toInt();
        if (ch >= 0 && ch < 16) {
          if (duty < 0)
            duty = 0;
          if (duty > 4095)
            duty = 4095;
          pca.setPWM(ch, 0, duty);
          float echo[2] = {(float)ch, (float)duty};
          writeFloats(0x33, echo, 2);
        }
      }
#endif
    }
  }
  if (readingEnabled)
    sendAllSensors();
  delay(mainDelay);
}
