# Примеры использования Rdk-HardwareLib

## RU

### Пример 1: Подключение к Arduino

```cpp
// Создание компонента подключения
auto arduinoConnect = storage->CreateComponent<UArduinoConnect>();
arduinoConnect->PortName = "COM3"; // или "/dev/ttyUSB0" на Linux
arduinoConnect->BaudRate = 9600;
arduinoConnect->Build();

// Подключение
if (arduinoConnect->Connect()) {
    // Arduino подключен
}
```

### Пример 2: Управление Arduino

```cpp
// Создание компонента управления
auto arduinoControl = storage->CreateComponent<UArduinoControl>();
arduinoControl->ArduinoConnection = arduinoConnect;
arduinoControl->Build();

// Отправка команды
arduinoControl->SendCommand("LED_ON");
```

### Пример 3: Работа с датчиком

```cpp
// Создание компонента датчика ADC
auto sensor = storage->CreateComponent<UAdcSensor>();
sensor->ArduinoControl = arduinoControl;
sensor->PinNumber = 0;
sensor->Build();

// Чтение значения
sensor->Calculate();
auto value = sensor->SensorValue;
```

---

## EN

### Example 1: Connecting to Arduino

```cpp
// Creating connection component
auto arduinoConnect = storage->CreateComponent<UArduinoConnect>();
arduinoConnect->PortName = "COM3"; // or "/dev/ttyUSB0" on Linux
arduinoConnect->BaudRate = 9600;
arduinoConnect->Build();

// Connecting
if (arduinoConnect->Connect()) {
    // Arduino connected
}
```

### Example 2: Arduino Control

```cpp
// Creating control component
auto arduinoControl = storage->CreateComponent<UArduinoControl>();
arduinoControl->ArduinoConnection = arduinoConnect;
arduinoControl->Build();

// Sending command
arduinoControl->SendCommand("LED_ON");
```

### Example 3: Sensor Operations

```cpp
// Creating ADC sensor component
auto sensor = storage->CreateComponent<UAdcSensor>();
sensor->ArduinoControl = arduinoControl;
sensor->PinNumber = 0;
sensor->Build();

// Reading value
sensor->Calculate();
auto value = sensor->SensorValue;
```
