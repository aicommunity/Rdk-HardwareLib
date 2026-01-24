# Rdk-HardwareLib - Документация

## RU

### Назначение

**Rdk-HardwareLib** предоставляет компоненты для работы с аппаратным обеспечением, в первую очередь с Arduino.

### Краткий обзор

Библиотека включает компоненты для:
- Подключения к Arduino
- Управления Arduino
- Работы с датчиками
- Демонстрационных контроллеров

### Быстрый старт

#### Подключение к Arduino

```cpp
// Создание компонента подключения
auto arduino = storage->CreateComponent<UArduinoConnect>("Arduino");
arduino->PortName = "COM3"; // или "/dev/ttyUSB0" на Linux
arduino->BaudRate = 9600;
arduino->Build();

// Подключение
if (arduino->Connect()) {
    // Arduino подключен
}
```

#### Чтение данных с датчика

```cpp
// Создание компонента датчика
auto sensor = storage->CreateComponent<UAdcSensor>("Sensor");
sensor->Arduino.AttachTo(&arduino->Output);
sensor->Pin = 0;
sensor->Build();
sensor->Calculate();
double value = sensor->Value();
```

### Связь с корневой документацией

Для обзорной информации см. корневую документацию проекта:
- `Docs/Libraries/Rdk-HardwareLib.md` - обзор библиотеки (в корневом репозитории)

### Детальная документация

- [Architecture.md](Architecture.md) - архитектура библиотеки
- [Usage-Examples.md](Usage-Examples.md) - примеры использования
- [API-Overview.md](API-Overview.md) - обзор API
- [Component-Catalog.md](Component-Catalog.md) - каталог компонентов

---

## EN

### Purpose

**Rdk-HardwareLib** provides components for working with hardware, primarily Arduino.

### Brief Overview

The library includes components for:
- Arduino connection
- Arduino control
- Sensor operations
- Demo controllers

### Link to Root Documentation

For overview information see root project documentation:
- `Docs/Libraries/Rdk-HardwareLib.md` - library overview (in root repository)

### Detailed Documentation

- [Architecture.md](Architecture.md) - library architecture
- [Usage-Examples.md](Usage-Examples.md) - usage examples
- [API-Overview.md](API-Overview.md) - API overview
- [Component-Catalog.md](Component-Catalog.md) - component catalog
