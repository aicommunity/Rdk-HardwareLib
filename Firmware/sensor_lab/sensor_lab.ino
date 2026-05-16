#include <DHT.h>
#include <Servo.h>

#define DEFAULT_DHTPIN 2
#define DHTTYPE DHT11
#define HALLPIN A2
#define SERVOPIN 9

DHT dht(DEFAULT_DHTPIN, DHTTYPE);
Servo myservo;

int* analogPins = nullptr;
int numPins = 0;
bool readingEnabled = true;
int dhtPin = DEFAULT_DHTPIN;
bool servoRunning = false;

int targetAngle = 90;
int currentAngle = 90;
unsigned long stepDelay = 50;
unsigned long lastStepTime = 0;
unsigned long mainDelay = 2000;

int getPinFromString(String pinStr);

void setup() {
  Serial.begin(57600);
  dht.begin();
  myservo.attach(SERVOPIN);
  pinMode(HALLPIN, INPUT);
}

void loop() {
  if (Serial.available() > 0) {
    String command = Serial.readStringUntil('\n');
    command.trim();

    if (command.startsWith("SET PINS COUNT")) {
      int newCount = command.substring(15).toInt();
      if (newCount > 0) {
        if (analogPins)
          free(analogPins);
        numPins = newCount;
        analogPins = (int*)malloc(numPins * sizeof(int));
        for (int i = 0; i < numPins; i++) {
          while (Serial.available() == 0)
            delay(10);
          String pinStr = Serial.readStringUntil('\n');
          int pin = getPinFromString(pinStr);
          if (pin != -1) {
            analogPins[i] = pin;
            pinMode(analogPins[i], INPUT);
          } else {
            i--;
          }
        }
        sendPinConfigPacket();
      }
    } else if (command.startsWith("SET DHT PIN")) {
      String pinStr = command.substring(12);
      int newDHTPin = getPinFromString(pinStr);
      if (newDHTPin != -1) {
        dhtPin = newDHTPin;
        dht = DHT(dhtPin, DHTTYPE);
        dht.begin();
      }
    } else if (command == "START READING") {
      readingEnabled = true;
    } else if (command == "STOP READING") {
      readingEnabled = false;
    } else if (command == "ROTATE") {
      servoRunning = true;
    } else if (command == "STOP ROTATE") {
      servoRunning = false;
    } else if (command.startsWith("SET SPEED")) {
      int newDelay = command.substring(10).toInt();
      if (newDelay > 0)
        stepDelay = newDelay;
    } else if (command.startsWith("SET DELAY")) {
      int newDelay = command.substring(10).toInt();
      if (newDelay > 0)
        mainDelay = newDelay;
    } else if (command == "GET STATUS") {
      sendStatusPacket();
    }
  }

  if (servoRunning)
    myservo.write(180);
  else
    myservo.write(90);

  if (currentAngle != targetAngle) {
    if (millis() - lastStepTime >= stepDelay) {
      currentAngle += (targetAngle > currentAngle) ? 1 : -1;
      myservo.write(currentAngle);
      lastStepTime = millis();
    }
  }

  if (readingEnabled) {
    if (analogPins) {
      for (int i = 0; i < numPins; i++)
        analogRead(analogPins[i]);
    }
    sendSensorData();
  }

  delay(mainDelay);
}

void sendPinConfigPacket() {
  Serial.write(0x02);
  Serial.write((uint8_t)numPins);
  for (int i = 0; i < numPins; i++)
    Serial.write((uint8_t)analogPins[i]);
}

void sendStatusPacket() {
  Serial.write(0x04);
  Serial.write((uint8_t)numPins);
  if (analogPins) {
    for (int i = 0; i < numPins; i++)
      Serial.write((uint8_t)analogPins[i]);
  }
  Serial.write((uint8_t)HALLPIN);
  Serial.write((uint8_t)dhtPin);
  Serial.write((uint8_t)SERVOPIN);
}

void sendSensorData() {
  float humidity = dht.readHumidity();
  float temperature = dht.readTemperature();
  int hallValue = analogRead(HALLPIN);
  float currentSpeed = (stepDelay > 0) ? 1000.0f / stepDelay : 0.0f;

  uint8_t errorFlags = 0;
  if (isnan(temperature)) errorFlags |= 0x01;
  if (isnan(humidity)) errorFlags |= 0x02;

  float hallStatus = NAN;
  if (hallValue < 10 || hallValue > 1013)
    errorFlags |= 0x04;
  else
    hallStatus = (hallValue > 512) ? 1.0f : 0.0f;

  Serial.write(0x01);
  Serial.write(errorFlags);
  Serial.write((uint8_t)4);
  Serial.write(reinterpret_cast<char*>(&temperature), sizeof(float));
  Serial.write(reinterpret_cast<char*>(&humidity), sizeof(float));
  Serial.write(reinterpret_cast<char*>(&hallStatus), sizeof(float));
  Serial.write(reinterpret_cast<char*>(&currentSpeed), sizeof(float));
}

int getPinFromString(String pinStr) {
  pinStr.trim();
  if (pinStr == "A0") return A0;
  if (pinStr == "A1") return A1;
  if (pinStr == "A2") return A2;
  if (pinStr == "A3") return A3;
  if (pinStr == "A4") return A4;
  if (pinStr == "A5") return A5;
  if (pinStr == "D2") return 2;
  if (pinStr == "D3") return 3;
  if (pinStr == "D4") return 4;
  if (pinStr == "D5") return 5;
  if (pinStr == "D6") return 6;
  if (pinStr == "D7") return 7;
  if (pinStr == "D8") return 8;
  if (pinStr == "D9") return 9;
  if (pinStr == "D10") return 10;
  if (pinStr == "D11") return 11;
  if (pinStr == "D12") return 12;
  if (pinStr == "D13") return 13;
  return -1;
}
