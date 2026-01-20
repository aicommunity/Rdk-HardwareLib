# Обзор API Rdk-HardwareLib

## RU

### Основные классы

#### HardwareLib

Главный класс библиотеки, наследник `ULibrary`.

#### UArduinoConnect

Подключение к Arduino.

**Основные свойства:**
- `PortName` - имя порта (COM3, /dev/ttyUSB0)
- `BaudRate` - скорость передачи данных
- `IsConnected` - статус подключения

**Основные методы:**
- `Connect()` - подключение к Arduino
- `Disconnect()` - отключение

#### UArduinoControl

Управление Arduino.

**Основные свойства:**
- `ArduinoConnection` - соединение с Arduino
- `CommandQueue` - очередь команд

**Основные методы:**
- `SendCommand(string)` - отправка команды
- `ReadResponse()` - чтение ответа

#### UAdcSensor

Датчик ADC.

**Основные свойства:**
- `ArduinoControl` - управление Arduino
- `PinNumber` - номер пина
- `SensorValue` - значение датчика

### См. также

- [Reports/05-Rdk-HardwareLib.md](../../../Reports/05-Rdk-HardwareLib.md) - детальное описание
- Исходный код: `Libraries/Rdk-HardwareLib/Core/`

---

## EN

### Main Classes

#### HardwareLib

Main library class, inherits from `ULibrary`.

#### UArduinoConnect

Arduino connection.

**Main Properties:**
- `PortName` - port name (COM3, /dev/ttyUSB0)
- `BaudRate` - baud rate
- `IsConnected` - connection status

**Main Methods:**
- `Connect()` - connect to Arduino
- `Disconnect()` - disconnect

#### UArduinoControl

Arduino control.

**Main Properties:**
- `ArduinoConnection` - Arduino connection
- `CommandQueue` - command queue

**Main Methods:**
- `SendCommand(string)` - send command
- `ReadResponse()` - read response

#### UAdcSensor

ADC sensor.

**Main Properties:**
- `ArduinoControl` - Arduino control
- `PinNumber` - pin number
- `SensorValue` - sensor value

### See Also

- [Reports/05-Rdk-HardwareLib.md](../../../Reports/05-Rdk-HardwareLib.md) - detailed description
- Source code: `Libraries/Rdk-HardwareLib/Core/`
