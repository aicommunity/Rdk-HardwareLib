/*
 * nmsdk_i2c_hub_v1 — framed v2 I2C hub (P2 Tier C / P2+).
 * Host plugin: nmsdk_i2c_hub_v1
 *
 * Frame types (payload = [0,n] + n×float LE):
 *   0x01 climate  — t, h, pressure_hpa           (BME280)
 *   0x30 ToF      — distance_mm                  (VL53L0X)
 *   0x31 IMU      — ax..gz                       (MPU6050)
 *   0x32 power    — bus_v, current_ma, power_mw  (INA219)
 *   0x33 PCA echo — ch, duty_12bit               (after SET PWM)
 *   0x34 climate2 — t, h, pressure_hpa           (BMP280 / BME680)
 *   0x35 ToF2     — distance_mm                  (VL53L1X)
 *   0x36 IMU2     — ax..gz[,mx,my]               (ICM-20948)
 *   0x37 env      — t, h                         (AHT20 / SHT31)
 *   0x38 lux      — lux                          (BH1750)
 *   0x39 ir temp  — object_c, ambient_c          (MLX90614)
 *   0x3A gas      — eco2, tvoc                   (SGP30)
 *   0x3B color    — r,g,b,c                      (TCS34725)
 *   0x3C accel    — ax,ay,az                     (ADXL345)
 *   0x3D gesture  — gesture, prox, r,g,b         (APDS-9960)
 *   0x3E adc      — ch0..ch3 volts               (ADS1115)
 *
 * Priority flags default 1; P2+ extras default 0 (flash budget).
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
#ifndef NMSDK_I2C_HUB_BMP280
#define NMSDK_I2C_HUB_BMP280 0
#endif
#ifndef NMSDK_I2C_HUB_BME680
#define NMSDK_I2C_HUB_BME680 0
#endif
#ifndef NMSDK_I2C_HUB_VL53L1
#define NMSDK_I2C_HUB_VL53L1 0
#endif
#ifndef NMSDK_I2C_HUB_ICM20948
#define NMSDK_I2C_HUB_ICM20948 0
#endif
#ifndef NMSDK_I2C_HUB_AHT20
#define NMSDK_I2C_HUB_AHT20 0
#endif
#ifndef NMSDK_I2C_HUB_SHT31
#define NMSDK_I2C_HUB_SHT31 0
#endif
#ifndef NMSDK_I2C_HUB_BH1750
#define NMSDK_I2C_HUB_BH1750 0
#endif
#ifndef NMSDK_I2C_HUB_MLX90614
#define NMSDK_I2C_HUB_MLX90614 0
#endif
#ifndef NMSDK_I2C_HUB_SGP30
#define NMSDK_I2C_HUB_SGP30 0
#endif
#ifndef NMSDK_I2C_HUB_TCS34725
#define NMSDK_I2C_HUB_TCS34725 0
#endif
#ifndef NMSDK_I2C_HUB_ADXL345
#define NMSDK_I2C_HUB_ADXL345 0
#endif
#ifndef NMSDK_I2C_HUB_APDS9960
#define NMSDK_I2C_HUB_APDS9960 0
#endif
#ifndef NMSDK_I2C_HUB_ADS1115
#define NMSDK_I2C_HUB_ADS1115 0
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
#if NMSDK_I2C_HUB_BMP280
#include <Adafruit_BMP280.h>
Adafruit_BMP280 bmp;
bool bmpOk = false;
#endif
#if NMSDK_I2C_HUB_BME680
#include <Adafruit_BME680.h>
Adafruit_BME680 bme680;
bool bme680Ok = false;
#endif
#if NMSDK_I2C_HUB_VL53L1
#include <Adafruit_VL53L1X.h>
Adafruit_VL53L1X vl53l1 = Adafruit_VL53L1X();
bool vl53l1Ok = false;
#endif
#if NMSDK_I2C_HUB_ICM20948
#include <Adafruit_ICM20948.h>
#include <Adafruit_ICM20X.h>
Adafruit_ICM20948 icm;
bool icmOk = false;
#endif
#if NMSDK_I2C_HUB_AHT20
#include <Adafruit_AHTX0.h>
Adafruit_AHTX0 aht;
bool ahtOk = false;
#endif
#if NMSDK_I2C_HUB_SHT31
#include <Adafruit_SHT31.h>
Adafruit_SHT31 sht31 = Adafruit_SHT31();
bool shtOk = false;
#endif
#if NMSDK_I2C_HUB_BH1750
#include <BH1750.h>
BH1750 bh1750;
bool bhOk = false;
#endif
#if NMSDK_I2C_HUB_MLX90614
#include <Adafruit_MLX90614.h>
Adafruit_MLX90614 mlx = Adafruit_MLX90614();
bool mlxOk = false;
#endif
#if NMSDK_I2C_HUB_SGP30
#include <Adafruit_SGP30.h>
Adafruit_SGP30 sgp;
bool sgpOk = false;
#endif
#if NMSDK_I2C_HUB_TCS34725
#include <Adafruit_TCS34725.h>
Adafruit_TCS34725 tcs = Adafruit_TCS34725();
bool tcsOk = false;
#endif
#if NMSDK_I2C_HUB_ADXL345
#include <Adafruit_ADXL345_U.h>
Adafruit_ADXL345_Unified adxl = Adafruit_ADXL345_Unified(12345);
bool adxlOk = false;
#endif
#if NMSDK_I2C_HUB_APDS9960
#include <Adafruit_APDS9960.h>
Adafruit_APDS9960 apds;
bool apdsOk = false;
#endif
#if NMSDK_I2C_HUB_ADS1115
#include <Adafruit_ADS1X15.h>
Adafruit_ADS1115 ads;
bool adsOk = false;
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

void sendClimateExtra()
{
  float v[3] = {0, 0, 0};
  bool any = false;
#if NMSDK_I2C_HUB_BMP280
  if (bmpOk) {
    v[0] = bmp.readTemperature();
    v[1] = 0;
    v[2] = bmp.readPressure() / 100.0f;
    any = true;
  }
#endif
#if NMSDK_I2C_HUB_BME680
  if (bme680Ok && bme680.performReading()) {
    v[0] = bme680.temperature;
    v[1] = bme680.humidity;
    v[2] = bme680.pressure / 100.0f;
    any = true;
  }
#endif
  if (any)
    writeFloats(0x34, v, 3);
}

void sendTofL1()
{
#if NMSDK_I2C_HUB_VL53L1
  float v[1] = {0};
  if (vl53l1Ok && vl53l1.dataReady()) {
    v[0] = (float)vl53l1.distance();
    vl53l1.clearInterrupt();
  }
  writeFloats(0x35, v, 1);
#endif
}

void sendIcm()
{
#if NMSDK_I2C_HUB_ICM20948
  float v[8] = {0, 0, 0, 0, 0, 0, 0, 0};
  if (icmOk) {
    sensors_event_t accel, gyro, mag, temp;
    icm.getEvent(&accel, &gyro, &temp, &mag);
    v[0] = accel.acceleration.x;
    v[1] = accel.acceleration.y;
    v[2] = accel.acceleration.z;
    v[3] = gyro.gyro.x;
    v[4] = gyro.gyro.y;
    v[5] = gyro.gyro.z;
    v[6] = mag.magnetic.x;
    v[7] = mag.magnetic.y;
  }
  writeFloats(0x36, v, 8);
#endif
}

void sendEnvTh()
{
  float v[2] = {0, 0};
  bool any = false;
#if NMSDK_I2C_HUB_AHT20
  if (ahtOk) {
    sensors_event_t humidity, temp;
    aht.getEvent(&humidity, &temp);
    v[0] = temp.temperature;
    v[1] = humidity.relative_humidity;
    any = true;
  }
#endif
#if NMSDK_I2C_HUB_SHT31
  if (shtOk) {
    v[0] = sht31.readTemperature();
    v[1] = sht31.readHumidity();
    any = true;
  }
#endif
  if (any)
    writeFloats(0x37, v, 2);
}

void sendLux()
{
#if NMSDK_I2C_HUB_BH1750
  float v[1] = {0};
  if (bhOk)
    v[0] = bh1750.readLightLevel();
  writeFloats(0x38, v, 1);
#endif
}

void sendMlx()
{
#if NMSDK_I2C_HUB_MLX90614
  float v[2] = {0, 0};
  if (mlxOk) {
    v[0] = mlx.readObjectTempC();
    v[1] = mlx.readAmbientTempC();
  }
  writeFloats(0x39, v, 2);
#endif
}

void sendSgp()
{
#if NMSDK_I2C_HUB_SGP30
  float v[2] = {0, 0};
  if (sgpOk && sgp.IAQmeasure()) {
    v[0] = (float)sgp.eCO2;
    v[1] = (float)sgp.TVOC;
  }
  writeFloats(0x3A, v, 2);
#endif
}

void sendTcs()
{
#if NMSDK_I2C_HUB_TCS34725
  float v[4] = {0, 0, 0, 0};
  if (tcsOk) {
    uint16_t r, g, b, c;
    tcs.getRawData(&r, &g, &b, &c);
    v[0] = r;
    v[1] = g;
    v[2] = b;
    v[3] = c;
  }
  writeFloats(0x3B, v, 4);
#endif
}

void sendAdxl()
{
#if NMSDK_I2C_HUB_ADXL345
  float v[3] = {0, 0, 0};
  if (adxlOk) {
    sensors_event_t event;
    adxl.getEvent(&event);
    v[0] = event.acceleration.x;
    v[1] = event.acceleration.y;
    v[2] = event.acceleration.z;
  }
  writeFloats(0x3C, v, 3);
#endif
}

void sendApds()
{
#if NMSDK_I2C_HUB_APDS9960
  float v[5] = {0, 0, 0, 0, 0};
  if (apdsOk) {
    if (apds.gestureValid())
      v[0] = (float)apds.readGesture();
    v[1] = (float)apds.readProximity();
    uint16_t r, g, b, c;
    if (apds.colorDataReady()) {
      apds.getColorData(&r, &g, &b, &c);
      v[2] = r;
      v[3] = g;
      v[4] = b;
    }
  }
  writeFloats(0x3D, v, 5);
#endif
}

void sendAds()
{
#if NMSDK_I2C_HUB_ADS1115
  float v[4] = {0, 0, 0, 0};
  if (adsOk) {
    v[0] = ads.computeVolts(ads.readADC_SingleEnded(0));
    v[1] = ads.computeVolts(ads.readADC_SingleEnded(1));
    v[2] = ads.computeVolts(ads.readADC_SingleEnded(2));
    v[3] = ads.computeVolts(ads.readADC_SingleEnded(3));
  }
  writeFloats(0x3E, v, 4);
#endif
}

void sendAllSensors()
{
  sendClimate();
  sendTof();
  sendImu();
  sendPower();
  sendClimateExtra();
  sendTofL1();
  sendIcm();
  sendEnvTh();
  sendLux();
  sendMlx();
  sendSgp();
  sendTcs();
  sendAdxl();
  sendApds();
  sendAds();
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
#if NMSDK_I2C_HUB_BMP280
  bmpOk = bmp.begin(0x76) || bmp.begin(0x77);
#endif
#if NMSDK_I2C_HUB_BME680
  bme680Ok = bme680.begin(0x77) || bme680.begin(0x76);
#endif
#if NMSDK_I2C_HUB_VL53L1
  vl53l1Ok = vl53l1.begin(0x29, &Wire);
  if (vl53l1Ok)
    vl53l1.startRanging();
#endif
#if NMSDK_I2C_HUB_ICM20948
  icmOk = icm.begin_I2C();
#endif
#if NMSDK_I2C_HUB_AHT20
  ahtOk = aht.begin();
#endif
#if NMSDK_I2C_HUB_SHT31
  shtOk = sht31.begin(0x44);
#endif
#if NMSDK_I2C_HUB_BH1750
  bhOk = bh1750.begin(BH1750::CONTINUOUS_HIGH_RES_MODE);
#endif
#if NMSDK_I2C_HUB_MLX90614
  mlxOk = mlx.begin();
#endif
#if NMSDK_I2C_HUB_SGP30
  sgpOk = sgp.begin();
  if (sgpOk)
    sgp.IAQinit();
#endif
#if NMSDK_I2C_HUB_TCS34725
  tcsOk = tcs.begin();
#endif
#if NMSDK_I2C_HUB_ADXL345
  adxlOk = adxl.begin();
#endif
#if NMSDK_I2C_HUB_APDS9960
  apdsOk = apds.begin();
  if (apdsOk) {
    apds.enableProximity(true);
    apds.enableGesture(true);
    apds.enableColor(true);
  }
#endif
#if NMSDK_I2C_HUB_ADS1115
  adsOk = ads.begin();
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
