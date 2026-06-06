# ArduinoConnect — вспомогательный класс подключения к Arduino (Rdk-HardwareLib)

> **Устарело:** `UArduinoConnect` удалён. Функции перенесены в `UArduinoSerialSession`, `UArduinoBinaryStreamParser` и `UArduinoCustomLink` / `UArduinoSensorSketch`. Актуальный каталог: [Component-Catalog.md](../Component-Catalog.md). Ниже — историческая документация.

## RU

### Назначение

**Класс (исторический)**: `UArduinoConnect` — вспомогательный класс для работы с последовательным портом Arduino.  
**Тип**: Вспомогательный класс (не Storage-компонент).  
**Использование**: Используется внутри `UArduinoControl` для управления последовательным портом.

`UArduinoConnect` наследуется от `QThread` и обеспечивает низкоуровневую работу с последовательным портом Arduino. Управляет подключением, отправкой команд, получением данных, буферизацией и парсингом пакетов данных от Arduino. Не является Storage-компонентом и не регистрируется через `UploadClass`, но является критически важной частью архитектуры библиотеки.

**Примечание:** Этот класс не является Storage-компонентом и не может быть создан напрямую через `storage->CreateComponent<>()`. Он используется внутри `UArduinoControl` и создается автоматически.

### UML-диаграмма классов

```mermaid
classDiagram
    QThread <|-- UArduinoConnect
    UArduinoConnect *-- QSerialPort : SerialPort
    UArduinoConnect *-- DataPoint : DataBuffer
    class QThread {
        +run() void
        +start() void
        +quit() void
    }
    class UArduinoConnect {
        +SerialPort : QSerialPort*
        +CurrentPortName : QString
        +DataBuffer : QVector~DataPoint~
        +allPins : QVector~int~
        +bufferMutex : QMutex
        +writeMutex : QMutex
        +commandMutex : QMutex
        +WriteBuffer : QByteArray
        +com : string
        +deb : bool
        +InitSerialPort(string) void
        +UploadArduino(QString) bool
        +OnSerialPortRead() void
        +pinToString(int) QString
        +CheckWrite() void
        +SetCommand(string) void
        +SendData() void
        +DateTime() double
        +FillData(...) void
        +GetAndClearAllData() QVector~double~
    }
    class QSerialPort {
        +open(QIODevice::OpenMode) bool
        +close() void
        +readAll() QByteArray
        +write(QByteArray) qint64
        +setBaudRate(int) void
    }
    class DataPoint {
        +data : QVector~double~
    }
```

**Иерархия наследования:**
- `QThread` — базовый класс Qt для многопоточности
- `UArduinoConnect` — класс подключения к Arduino

**Связи:**
- `UArduinoConnect` использует `QSerialPort` для работы с последовательным портом
- `UArduinoConnect` хранит данные в `DataBuffer` (вектор `DataPoint`)
- `UArduinoControl` использует `UArduinoConnect` (композиция)

### UML-диаграмма последовательности

```mermaid
sequenceDiagram
    participant Control as UArduinoControl
    participant Connect as UArduinoConnect
    participant Port as QSerialPort
    participant Arduino as Arduino Board
    
    Control->>Connect: new UArduinoConnect(PortName)
    Connect->>Connect: InitSerialPort(PortName)
    Connect->>Port: new QSerialPort(PortName)
    Connect->>Port: setBaudRate(9600)
    Connect->>Port: open(QIODevice::ReadWrite)
    Port-->>Connect: Port opened
    Connect->>Port: connect(readyRead signal)
    Note over Connect: Готов к работе
    
    loop Каждый шаг
        Control->>Connect: SetCommand("COMMAND")
        Control->>Connect: SendData()
        Connect->>Connect: Проверка com
        Connect->>Port: write(command + '\n')
        Port->>Arduino: Отправка команды
        Arduino-->>Port: Данные (пакеты)
        Port->>Connect: readyRead signal
        Connect->>Connect: OnSerialPortRead()
        Connect->>Connect: Парсинг пакетов
        alt Packet ID 0x01 (Sensor Data)
            Connect->>Connect: FillData(timestamp, params...)
            Connect->>Connect: DataBuffer.append(DataPoint)
        else Packet ID 0x02 (Pin Info)
            Connect->>Connect: allPins = newPins
        else Packet ID 0x03 (Error)
            Connect->>Connect: Обработка ошибки
        else Packet ID 0x04 (Status)
            Connect->>Connect: Вывод статуса
        end
    end
    
    Control->>Connect: GetAndClearAllData()
    Connect->>Connect: Извлечение всех данных из DataBuffer
    Connect->>Connect: DataBuffer.clear()
    Connect-->>Control: QVector~double~ data
```

**Жизненный цикл:**
1. **Создание** — объект создается в `UArduinoControl::ACalculate()` при первом вызове
2. **Инициализация** — вызов `InitSerialPort()` для открытия последовательного порта
3. **Работа** — в отдельном потоке (наследуется от `QThread`):
   - Отправка команд через `SendData()`
   - Получение данных через сигнал `readyRead`
   - Парсинг пакетов в `OnSerialPortRead()`
   - Буферизация данных в `DataBuffer`
4. **Получение данных** — `UArduinoControl` вызывает `GetAndClearAllData()` для получения данных
5. **Уничтожение** — объект уничтожается в `UArduinoControl::AUnInit()`

### UML-диаграмма состояний

```mermaid
stateDiagram-v2
    [*] --> Created: new UArduinoConnect()
    Created --> Initializing: InitSerialPort()
    Initializing --> CheckingPort: Проверка существования порта
    CheckingPort -->|Порт не найден| Error: Порт не существует
    CheckingPort -->|Порт найден| Opening: Открытие порта
    Opening -->|Ошибка открытия| Error: Не удалось открыть
    Opening -->|Успешно| Connected: Порт открыт
    Connected --> Listening: Готов к работе
    Listening --> Receiving: readyRead signal
    Receiving --> Parsing: OnSerialPortRead()
    Parsing --> ProcessingPacket: Определение типа пакета
    ProcessingPacket -->|0x01| StoringSensorData: FillData()
    ProcessingPacket -->|0x02| UpdatingPins: Обновление allPins
    ProcessingPacket -->|0x03| HandlingError: Обработка ошибки
    ProcessingPacket -->|0x04| ShowingStatus: Вывод статуса
    StoringSensorData --> Listening
    UpdatingPins --> Listening
    HandlingError --> Listening
    ShowingStatus --> Listening
    Listening --> Sending: SendData()
    Sending --> Writing: Запись в порт
    Writing --> Listening
    Listening --> Closing: Деструктор
    Closing --> [*]
    Error --> [*]
```

**Состояния:**
- **Created** — объект создан
- **Initializing** — выполняется инициализация порта
- **CheckingPort** — проверка существования порта
- **Opening** — открытие последовательного порта
- **Connected** — порт открыт и готов к работе
- **Listening** — ожидание данных и команд
- **Receiving** — получение данных от Arduino
- **Parsing** — парсинг полученных данных
- **ProcessingPacket** — определение типа пакета
- **StoringSensorData** — сохранение данных датчиков
- **UpdatingPins** — обновление конфигурации пинов
- **HandlingError** — обработка ошибок
- **ShowingStatus** — вывод статуса
- **Sending** — отправка команды
- **Writing** — запись данных в порт
- **Closing** — закрытие порта и уничтожение объекта
- **Error** — состояние ошибки

### UML-диаграмма активности

```mermaid
flowchart TD
    Start([Start OnSerialPortRead]) --> ReadData[Read all data from SerialPort]
    ReadData --> InitIndex[index = 0]
    InitIndex --> CheckIndex{index < dataSize?}
    CheckIndex -->|Нет| End([End])
    CheckIndex -->|Да| ReadPacketId[Read packetId]
    ReadPacketId --> CheckPacketId{Packet ID?}
    
    CheckPacketId -->|0x01| SensorPacket[Read errorFlags, paramCount]
    SensorPacket --> ValidateSensor{Достаточно данных?}
    ValidateSensor -->|Нет| End
    ValidateSensor -->|Да| ReadValues[Read float values]
    ReadValues --> CheckErrors{Есть ошибки?}
    CheckErrors -->|Да| LogErrors[Логирование ошибок]
    CheckErrors -->|Нет| FillData[FillData timestamp, params]
    LogErrors --> FillData
    FillData --> CheckBuffer{DataBuffer.size >= 512?}
    CheckBuffer -->|Да| RemoveOld[Remove oldest]
    CheckBuffer -->|Нет| AppendData[Append DataPoint]
    RemoveOld --> AppendData
    AppendData --> CheckIndex
    
    CheckPacketId -->|0x02| PinPacket[Read pinCount]
    PinPacket --> ValidatePin{Достаточно данных?}
    ValidatePin -->|Нет| End
    ValidatePin -->|Да| ReadPins[Read pin values]
    ReadPins --> UpdatePins[allPins = newPins]
    UpdatePins --> CheckIndex
    
    CheckPacketId -->|0x03| ErrorPacket[Read errorCode]
    ErrorPacket --> ProcessError[Обработка ошибки]
    ProcessError --> CheckIndex
    
    CheckPacketId -->|0x04| StatusPacket[Read analogPinCount]
    StatusPacket --> ValidateStatus{Достаточно данных?}
    ValidateStatus -->|Нет| End
    ValidateStatus -->|Да| ReadStatus[Read pins, DHT pin, servo pin]
    ReadStatus --> ShowStatus[Вывод статуса]
    ShowStatus --> CheckIndex
    
    CheckPacketId -->|Другое| UnknownPacket[Логирование неизвестного пакета]
    UnknownPacket --> CheckIndex
```

**Алгоритм работы OnSerialPortRead():**
1. Чтение всех данных из последовательного порта
2. Парсинг пакетов по байтам:
   - Чтение ID пакета (первый байт)
   - Обработка в зависимости от типа пакета:
     - **0x01** — данные датчиков: чтение флагов ошибок, количества параметров, значений float, сохранение в буфер
     - **0x02** — информация о пинах: чтение количества пинов, значений пинов, обновление `allPins`
     - **0x03** — ошибки: чтение кода ошибки, обработка
     - **0x04** — статус: чтение конфигурации пинов, вывод статуса
3. Продолжение парсинга до конца данных

### UML-диаграмма компонентов

```mermaid
graph TB
    subgraph RdkHardwareLib["Rdk-HardwareLib"]
        ArduinoConnect[UArduinoConnect]
        ArduinoControl[UArduinoControl]
    end
    
    subgraph Qt5["Qt5 Libraries"]
        QtCore[Qt5::Core]
        QtSerialPort[Qt5::SerialPort]
        QtThread[Qt5::Core QThread]
    end
    
    ArduinoConnect -->|наследуется| QtThread
    ArduinoConnect -->|использует| QtCore
    ArduinoConnect -->|использует| QtSerialPort
    ArduinoControl -->|использует| ArduinoConnect
    
    subgraph Interfaces["Интерфейсы"]
        SerialPort[QSerialPort<br/>Последовательный порт]
        DataBuffer[DataBuffer<br/>Буфер данных]
        Signals[Qt Signals<br/>Сигналы]
    end
    
    ArduinoConnect --> SerialPort
    ArduinoConnect --> DataBuffer
    ArduinoConnect --> Signals
```

**Зависимости:**
- **Qt5::Core** — базовые классы Qt (`QThread`, `QObject`, `QMutex`, `QDateTime`)
- **Qt5::SerialPort** — работа с последовательными портами (`QSerialPort`, `QSerialPortInfo`)

**Интерфейсы:**
- **Последовательный порт** — работа через `QSerialPort`
- **Буфер данных** — хранение данных в `DataBuffer` (вектор `DataPoint`)
- **Qt Signals** — сигналы для асинхронной работы (`readyRead`, `UploadFinished`, `DataReceived`, `SerialPortConnected`)

### Структура данных

#### DataPoint

Структура для хранения одной точки данных:

```cpp
struct DataPoint {
    QVector<double> data;  // Формат: [timestamp, paramCount, param1, param2, ...]
};
```

**Формат данных:**
- `data[0]` — timestamp (время получения данных)
- `data[1]` — paramCount (количество параметров)
- `data[2]` — param1 (первый параметр, например, temperature)
- `data[3]` — param2 (второй параметр, например, humidity)
- `data[4]` — param3 (третий параметр, например, mfield)
- `data[5]` — param4 (четвертый параметр, например, servo_speed)

### Свойства и поля

#### Публичные поля

- **`SerialPort`** (QSerialPort*) — указатель на объект последовательного порта. Создается в `InitSerialPort()`, уничтожается в деструкторе.

- **`CurrentPortName`** (QString) — имя текущего COM-порта (например, "COM3", "/dev/ttyUSB0").

- **`DataBuffer`** (QVector<DataPoint>) — буфер для хранения данных от Arduino. Максимальный размер: 512 точек данных. При переполнении удаляется самая старая точка.

- **`allPins`** (QVector<int>) — вектор конфигурации пинов Arduino. Обновляется при получении пакета типа 0x02.

- **`bufferMutex`** (QMutex) — мьютекс для синхронизации доступа к `DataBuffer`.

- **`writeMutex`** (QMutex) — мьютекс для синхронизации записи в порт.

- **`commandMutex`** (QMutex) — мьютекс для синхронизации доступа к команде `com`.

- **`WriteBuffer`** (QByteArray) — буфер для записи команд в порт. Максимальный размер: 4096 байт.

- **`com`** (string) — текущая команда для отправки. Устанавливается через `SetCommand()`, очищается после отправки.

- **`deb`** (bool) — флаг отладочного вывода. Устанавливается из `UArduinoControl->ShowDebug`.

### Методы

#### Публичные методы

- **`InitSerialPort(string &PortName)`** → `void` — инициализирует последовательный порт. Проверяет существование порта, создает `QSerialPort`, устанавливает скорость 9600 бод, открывает порт для чтения/записи, подключает сигнал `readyRead` к `OnSerialPortRead()`. Параметры:
  - `PortName` (string&) — имя COM-порта

- **`UploadArduino(const QString &FileName)`** → `bool` — загружает прошивку на Arduino через avrdude. Использует `QProcess` для запуска команды avrdude. Параметры:
  - `FileName` (const QString&) — путь к файлу прошивки (.hex)
  
  Возвращает: `true` при успешной загрузке, `false` при ошибке

- **`OnSerialPortRead()`** → `void` — обработчик сигнала `readyRead` от `QSerialPort`. Читает данные из порта, парсит пакеты по протоколу и обрабатывает их в зависимости от типа пакета.

- **`pinToString(int pin)`** → `QString` — преобразует номер пина в строковое представление. Для аналоговых пинов (14-19) возвращает "A0"-"A5", для цифровых (2-13) возвращает "D2"-"D13". Параметры:
  - `pin` (int) — номер пина

- **`CheckWrite()`** → `void` — проверяет возможность записи в порт и отправляет данные из `WriteBuffer`, если порт готов.

- **`SetCommand(string& cmd)`** → `void` — устанавливает команду для отправки. Команда сохраняется в `com` и отправляется при вызове `SendData()`. Параметры:
  - `cmd` (string&) — команда для отправки

- **`SendData()`** → `void` — отправляет команду из `com` на Arduino. Преобразует команду в UTF-8, добавляет символ новой строки, добавляет в `WriteBuffer` и вызывает `CheckWrite()` для отправки.

- **`DateTime()`** → `double` — возвращает текущее время в формате HHMMSS (часы, минуты, секунды). Используется для timestamp в данных датчиков.

- **`FillData(double timestamp, uint8_t paramCount, float temperature, float humidity, float mfield, float servo_speed)`** → `void` — добавляет точку данных в буфер. Создает `DataPoint`, заполняет данными и добавляет в `DataBuffer`. При переполнении буфера (>= 512 точек) удаляет самую старую точку. Параметры:
  - `timestamp` (double) — время получения данных
  - `paramCount` (uint8_t) — количество параметров
  - `temperature` (float) — температура
  - `humidity` (float) — влажность
  - `mfield` (float) — магнитное поле
  - `servo_speed` (float) — скорость сервопривода

- **`GetAndClearAllData()`** → `QVector<double>` — извлекает все данные из `DataBuffer` и очищает буфер. Возвращает вектор всех данных из всех точек данных. Используется `UArduinoControl` для получения данных.

#### Конструктор и деструктор

- **`UArduinoConnect(string &PortName)`** — конструктор. Инициализирует `SerialPort = nullptr`, `com = ""`, `deb = ""`, вызывает `InitSerialPort(PortName)`.

- **`~UArduinoConnect()`** — деструктор. Закрывает последовательный порт, если он открыт, и освобождает память.

### Протокол обмена данными

Класс использует бинарный протокол обмена данными с Arduino:

#### Типы пакетов

1. **0x01 — Данные датчиков**
   - Структура: `[packetId(1), errorFlags(1), paramCount(1), param1(4), param2(4), ...]`
   - `errorFlags`: битовые флаги ошибок (0x01 — температура, 0x02 — влажность, 0x04 — датчик Холла)
   - `paramCount`: количество параметров (1-4)
   - Параметры: float значения (4 байта каждое)

2. **0x02 — Информация о пинах**
   - Структура: `[packetId(1), pinCount(1), pin1(1), pin2(1), ...]`
   - `pinCount`: количество пинов
   - Пины: номера пинов (1 байт каждый)

3. **0x03 — Ошибки**
   - Структура: `[packetId(1), errorCode(1)]`
   - `errorCode`: код ошибки (0x01 — DHT Sensor Failure, 0x02 — Hall Sensor Failure, 0x03 — Analog Sensor Failure)

4. **0x04 — Статус**
   - Структура: `[packetId(1), analogPinCount(1), analogPin1(1), ..., dhtPin(1), servoPin(1)]`
   - `analogPinCount`: количество аналоговых пинов
   - Аналоговые пины: номера пинов
   - `dhtPin`: номер пина DHT датчика
   - `servoPin`: номер пина сервопривода

#### Отправка команд

Команды отправляются в текстовом формате UTF-8 с символом новой строки (`\n`) в конце. Команды буферизуются в `WriteBuffer` и отправляются асинхронно.

### Примеры использования

**Примечание:** `UArduinoConnect` не используется напрямую в коде приложения. Он создается и управляется внутри `UArduinoControl`. Примеры показаны для понимания внутренней работы.

#### Пример 1: Создание и использование (внутри UArduinoControl)

```cpp
// Внутри UArduinoControl::ACalculate()
if (UArdConn == nullptr) {
    string portName = PortToConnect;
    UArdConn = new UArduinoConnect(portName);
    // UArduinoConnect автоматически инициализирует порт в конструкторе
}

// Установка команды
UArdConn->SetCommand("READ_SENSORS");

// Отправка команды
UArdConn->SendData();

// Получение данных
QVector<double> allData = UArdConn->GetAndClearAllData();
```

#### Пример 2: Обработка данных

```cpp
// Данные приходят автоматически через сигнал readyRead
// Внутри OnSerialPortRead() происходит парсинг пакетов

// После парсинга данные сохраняются в DataBuffer
// UArduinoControl получает данные через GetAndClearAllData()
```

### Использование в конфигурациях

`UArduinoConnect` не используется напрямую в XML-конфигурациях, так как не является Storage-компонентом. Он создается автоматически внутри `UArduinoControl` при первом вызове `ACalculate()`.

### См. также

- [`UArduinoControl`](Arduino.md) — компонент управления Arduino (использует `UArduinoConnect`)
- [Architecture.md](../Architecture.md) — архитектура библиотеки

---

## EN

### Purpose

**Class**: `UArduinoConnect` — helper class for Arduino serial port communication.  
**Type**: Helper class (not a Storage component).  
**Usage**: Used internally by `UArduinoControl` for serial port management.

`UArduinoConnect` extends `QThread` and provides low-level serial port communication with Arduino. Manages connection, command sending, data receiving, buffering, and packet parsing. Not a Storage component and not registered via `UploadClass`, but is a critical part of the library architecture.

**Note:** This class is not a Storage component and cannot be created directly via `storage->CreateComponent<>()`. It is used internally by `UArduinoControl` and created automatically.

### UML Class Diagram

```mermaid
classDiagram
    QThread <|-- UArduinoConnect
    UArduinoConnect *-- QSerialPort : SerialPort
    class QThread {
        +run() void
    }
    class UArduinoConnect {
        +SerialPort : QSerialPort*
        +DataBuffer : QVector~DataPoint~
        +InitSerialPort(string) void
        +SendData() void
        +GetAndClearAllData() QVector~double~
    }
```

### UML Sequence Diagram

```mermaid
sequenceDiagram
    participant Control as UArduinoControl
    participant Connect as UArduinoConnect
    participant Port as QSerialPort
    
    Control->>Connect: new UArduinoConnect()
    Connect->>Port: InitSerialPort()
    Port-->>Connect: Port opened
    Control->>Connect: SetCommand()
    Control->>Connect: SendData()
    Connect->>Port: Write command
    Port->>Connect: readyRead signal
    Connect->>Connect: OnSerialPortRead()
    Connect->>Connect: Parse packets
    Control->>Connect: GetAndClearAllData()
    Connect-->>Control: Data vector
```

### UML State Diagram

```mermaid
stateDiagram-v2
    [*] --> Created: new()
    Created --> Initializing: InitSerialPort()
    Initializing --> Connected: Port opened
    Connected --> Listening: Ready
    Listening --> Receiving: Data received
    Receiving --> Parsing: Parse packets
    Parsing --> Listening
    Listening --> Sending: Send command
    Sending --> Listening
```

### UML Activity Diagram

```mermaid
flowchart TD
    Start([Start OnSerialPortRead]) --> ReadData[Read data]
    ReadData --> ParsePackets[Parse packets]
    ParsePackets --> ProcessPacket{Packet type?}
    ProcessPacket -->|0x01| StoreSensorData[Store sensor data]
    ProcessPacket -->|0x02| UpdatePins[Update pins]
    ProcessPacket -->|0x03| HandleError[Handle error]
    ProcessPacket -->|0x04| ShowStatus[Show status]
    StoreSensorData --> End([End])
    UpdatePins --> End
    HandleError --> End
    ShowStatus --> End
```

### Properties and Fields

#### Public Fields

- **`SerialPort`** (QSerialPort*) — pointer to serial port object
- **`CurrentPortName`** (QString) — current COM port name
- **`DataBuffer`** (QVector<DataPoint>) — buffer for storing data from Arduino (max 512 points)
- **`allPins`** (QVector<int>) — vector of pin configuration
- **`bufferMutex`** (QMutex) — mutex for `DataBuffer` synchronization
- **`writeMutex`** (QMutex) — mutex for write synchronization
- **`commandMutex`** (QMutex) — mutex for command synchronization
- **`WriteBuffer`** (QByteArray) — buffer for writing commands (max 4096 bytes)
- **`com`** (string) — current command to send
- **`deb`** (bool) — debug output flag

### Methods

#### Public Methods

- **`InitSerialPort(string &PortName)`** → `void` — initializes serial port
- **`UploadArduino(const QString &FileName)`** → `bool` — uploads firmware to Arduino via avrdude
- **`OnSerialPortRead()`** → `void` — handler for `readyRead` signal, parses incoming packets
- **`pinToString(int pin)`** → `QString` — converts pin number to string representation
- **`CheckWrite()`** → `void` — checks if port is ready and sends data from `WriteBuffer`
- **`SetCommand(string& cmd)`** → `void` — sets command to send
- **`SendData()`** → `void` — sends command from `com` to Arduino
- **`DateTime()`** → `double` — returns current time as HHMMSS
- **`FillData(...)`** → `void` — adds data point to buffer
- **`GetAndClearAllData()`** → `QVector<double>` — extracts all data from buffer and clears it

### Communication Protocol

Binary protocol for data exchange with Arduino:

- **0x01** — Sensor data: `[packetId, errorFlags, paramCount, param1(float), param2(float), ...]`
- **0x02** — Pin information: `[packetId, pinCount, pin1, pin2, ...]`
- **0x03** — Errors: `[packetId, errorCode]`
- **0x04** — Status: `[packetId, analogPinCount, analogPins..., dhtPin, servoPin]`

Commands are sent as UTF-8 text with newline character (`\n`) at the end.

### See also

- [`UArduinoControl`](Arduino.md) — Arduino control component (uses `UArduinoConnect`)
- [Architecture.md](../Architecture.md) — library architecture
