#include <DHT.h>
#include <Servo.h>

#define DEFAULT_DHTPIN 2  // Пин по умолчанию для датчика DHT11
#define DHTTYPE DHT11     // Тип датчика
#define HALLPIN A2        // Пин для датчика Холла (используем аналоговый пин)
#define SERVOPIN 9        // Пин для сервопривода

DHT dht(DEFAULT_DHTPIN, DHTTYPE);
Servo myservo;

int* analogPins;  // Указатель на массив пинов
int numPins = 2;   // Начальное количество пинов
bool readingEnabled = true; // Флаг для управления чтением
int dhtPin = DEFAULT_DHTPIN; // Пин для датчика DHT11
bool servoRunning = false; // Флаг для управления состоянием сервопривода

int targetAngle = 90;      // Целевой угол поворота
int currentAngle = 90;     // Текущий угол сервопривода
unsigned long stepDelay = 50; // Задержка между шагами (по умолчанию 50мс)
unsigned long lastStepTime = 0; // Время последнего шага
unsigned long mainDelay = 2000; // Задержка в основном цикле

void setup() {
  Serial.begin(9600); 
  dht.begin(); 
  myservo.attach(SERVOPIN); 

  while (!Serial) {
    ; 
  }

  analogPins = (int*)malloc(numPins * sizeof(int));
  analogPins[0] = A0;  
  analogPins[1] = A1;  

  for (int i = 0; i < numPins; i++) {
    pinMode(analogPins[i], INPUT);
  }

  pinMode(HALLPIN, INPUT);
}

void loop() {
  if (Serial.available() > 0) {
    String command = Serial.readStringUntil('\n'); 

    if (command.startsWith("SET PINS COUNT")) {
      int newCount = command.substring(15).toInt(); 
      if (newCount > 0) {
        free(analogPins);
        numPins = newCount;
        analogPins = (int*)malloc(numPins * sizeof(int));
        for (int i = 0; i < numPins; i++) {
          while (Serial.available() == 0) { 
            delay(10);
          }
          String pinStr = Serial.readStringUntil('\n');
          int pin = getPinFromString(pinStr);
          if (pin != -1) {
            analogPins[i] = pin;
            pinMode(analogPins[i], INPUT); 
          } else {
            i--; 
          }
        }
      }
    } else if (command.startsWith("SET DHT PIN")) {
      String pinStr = command.substring(12);
      int newDHTPin = getPinFromString(pinStr);
      if (newDHTPin != -1) {
        dhtPin = newDHTPin;
        dht = DHT(dhtPin, DHTTYPE);
        dht.begin();
      }
    } else if (command.startsWith("START READING")) {
      readingEnabled = true;
    } else if (command.startsWith("STOP READING")) {
      readingEnabled = false;
    } else if (command.startsWith("ROTATE")) {
      servoRunning = true;
    } else if (command.startsWith("STOP ROTATE")) {
      servoRunning = false;
    } else if (command.startsWith("SET SPEED")) {
      int newDelay = command.substring(10).toInt(); 
      if (newDelay > 0) {
        stepDelay = newDelay; 
      }
    } else if (command.startsWith("SET DELAY ")) { // Обработка SET DELAY
      int newDelay = command.substring(10).toInt();
      if (newDelay > 0) {
        mainDelay = newDelay;
      }
    } else if (command.startsWith("GET ENABLED PINS")) {
      sendEnabledPins(); 
    }
  }

  if (servoRunning) {
    myservo.write(180); 
  } else {
    myservo.write(90); 
  }

  if (currentAngle != targetAngle) {
    if (millis() - lastStepTime >= stepDelay) {
      currentAngle += (targetAngle > currentAngle) ? 1 : -1;
      myservo.write(currentAngle);
      lastStepTime = millis();
    }
  }

  if (readingEnabled) {
    for (int i = 0; i < numPins; i++) {
      int sensorValue = analogRead(analogPins[i]); 
    }
  }

  sendSensorData();

  delay(mainDelay); // Используется настраиваемая задержка
}

// Остальные функции (sendSensorData, sendEnabledPins, getPinFromString) остаются без изменений

void sendSensorData() {
  float humidity = dht.readHumidity();
  float temperature = dht.readTemperature();

  if (!isnan(temperature) && !isnan(humidity)) {
    int hallValue = analogRead(HALLPIN);
    float hallStatus = (hallValue > 512) ? 1.0 : 0.0;
    float currentSpeed = (float)stepDelay;

    // Пакет данных датчиков (ID 0x01)
    Serial.write(0x01); // Идентификатор пакета
    Serial.write(4);    // Количество параметров (4)
    Serial.write(reinterpret_cast<const char*>(&temperature), sizeof(float));
    Serial.write(reinterpret_cast<const char*>(&humidity), sizeof(float));
    Serial.write(reinterpret_cast<const char*>(&hallStatus), sizeof(float));
    Serial.write(reinterpret_cast<const char*>(&currentSpeed), sizeof(float));

    // Пакет информации о пинах (ID 0x02)
    Serial.write(0x02); // Идентификатор пакета
    Serial.write(static_cast<uint8_t>(numPins + 3)); // Общее количество пинов
    for(int i = 0; i < numPins; i++) {
      Serial.write(static_cast<uint8_t>(analogPins[i]));
    }
    Serial.write(static_cast<uint8_t>(dhtPin));
    Serial.write(static_cast<uint8_t>(HALLPIN));
    Serial.write(static_cast<uint8_t>(SERVOPIN));
  }
}

void sendEnabledPins() {
  const uint8_t packetHeader = 0xAA;
  Serial.write(packetHeader);

  Serial.write(static_cast<uint8_t>(numPins));

  for(int i = 0; i < numPins; i++) {
    Serial.write(static_cast<uint8_t>(analogPins[i]));
  }

  Serial.write(static_cast<uint8_t>(dhtPin));
  Serial.write(static_cast<uint8_t>(HALLPIN));
  Serial.write(static_cast<uint8_t>(SERVOPIN));
}

// Функция для преобразования строкового представления пина в номер пина
int getPinFromString(String pinStr) {
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
  return -1;  // Если пин не найден
}