/*
 * nmsdk_sensor_hub_v1 — tier C hub for DHT / HC-SR04 / analog on Sensor Shield.
 * Protocol: framed v2 (see Firmware/templates/framed_v2_hub).
 * Host plugin id: nmsdk_sensor_hub_v1 (register when HEX ships).
 */
#include <Arduino.h>

#ifndef NMSDK_SENSOR_HUB_BAUD
#define NMSDK_SENSOR_HUB_BAUD 57600
#endif

void setup()
{
  Serial.begin(NMSDK_SENSOR_HUB_BAUD);
}

void loop()
{
  // Stub: implement DHT/HC-SR04 sampling + framed TX; parse host commands.
  delay(50);
}
