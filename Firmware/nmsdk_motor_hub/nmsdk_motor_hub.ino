/*
 * nmsdk_motor_hub_v1 — tier C motor control for Motor Shield R3 / Seeed.
 */
#include <Arduino.h>

#ifndef NMSDK_MOTOR_HUB_BAUD
#define NMSDK_MOTOR_HUB_BAUD 57600
#endif

void setup()
{
  Serial.begin(NMSDK_MOTOR_HUB_BAUD);
}

void loop()
{
  delay(20);
}
