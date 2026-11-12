# DC — управление DC-двигателем (Rdk-HardwareLib)

## RU

### Назначение

**Класс**: `UDcControlDemo` — демонстрационный компонент для управления DC-двигателем через Arduino.  
**Регистрация**: `UHardwareLibrary.cpp` → `UploadClass("DC", ...)`.  
**Storage-инстансы**: `ClassName = "DC"` в `Bin/Configs/*/Model_*.xml`.

`UDcControlDemo` расширяет `UNet` функциональностью управления DC-двигателем через Arduino. Позволяет отправлять команды управления двигателем (скорость, направление), получать обратную связь по скорости и ускорению. Использует `UArduinoControl` для отправки команд на Arduino.

### UML-диаграмма классов

```mermaid
classDiagram
    UNet <|-- UDcControlDemo
    UDcControlDemo *-- UArduinoControl : UArdContr
    class UNet {
        +ADefault() bool
        +ABuild() bool
        +AReset() bool
        +ACalculate() bool
        +AInit() void
        +AUnInit() void
    }
    class UDcControlDemo {
        +Command : string
        +SendCommandFlag : bool
        +SentCommand : string
        +Speed : float
        +Acceleration : float
        +GetSpeed : bool
        -UArdContr : UEPtr~UArduinoControl~
        +New() UDcControlDemo*
        #ADefault() bool
        #ABuild() bool
        #AReset() bool
        #ACalculate() bool
        #AInit() void
        #AUnInit() void
    }
    class UArduinoControl {
        +InputCommand : string
        +SpeedValues : QVector~double~
    }
```

**Иерархия наследования:**
- `UNet` — базовый класс сетевого компонента из Rdk-BasicLib
- `UDcControlDemo` — компонент управления DC-двигателем

**Связи с другими компонентами:**
- `UDcControlDemo` использует `UArduinoControl` (композиция через `UEPtr`) для отправки команд на Arduino
- `UArduinoControl` предоставляет данные о скорости через `SpeedValues`

### UML-диаграмма последовательности

```mermaid
sequenceDiagram
    participant Storage as UStorage
    participant DC as UDcControlDemo
    participant Arduino as UArduinoControl
    participant Connect as UArduinoConnect
    
    Storage->>DC: New()
    Storage->>DC: Default()
    DC->>DC: ADefault()
    Note over DC: Инициализация параметров<br/>по умолчанию
    Storage->>DC: SetCommand("SET_SPEED 50")
    Storage->>DC: Build()
    DC->>DC: ABuild()
    Note over DC: Инициализация структуры
    DC-->>Storage: Ready = true
    loop Каждый шаг вычислений
        Storage->>DC: Calculate()
        DC->>DC: ACalculate()
        alt UArdContr == nullptr
            DC->>Arduino: new UArduinoControl()
            Note over Arduino: Создание компонента<br/>управления Arduino
        end
        alt SendCommandFlag == true
            DC->>Arduino: InputCommand = Command
            DC->>DC: SentCommand = Command
            DC->>DC: SendCommandFlag = false
            Arduino->>Connect: Отправка команды на Arduino
        end
        alt GetSpeed == true
            DC->>Arduino: Проверка SpeedValues
            alt SpeedValues не пуст
                Arduino-->>DC: SpeedValues.last()
                DC->>DC: Speed = SpeedValues.last()
            end
            DC->>DC: GetSpeed = false
        end
    end
    Storage->>DC: Reset()
    DC->>DC: AReset()
    Note over DC: Сброс состояния
```

**Жизненный цикл:**
1. **Создание** — компонент создается через конструктор
2. **Инициализация** — установка параметров по умолчанию через `ADefault()`
3. **Настройка** — установка команды управления через `Command`
4. **Сборка** — инициализация структуры через `ABuild()`
5. **Вычисления** — в каждом шаге:
   - Проверка и создание `UArduinoControl` при необходимости
   - Отправка команды через `InputCommand` (если `SendCommandFlag == true`)
   - Получение скорости из `UArduinoControl->SpeedValues` (если `GetSpeed == true`)
6. **Сброс** — сброс состояния через `AReset()`

### UML-диаграмма состояний

```mermaid
stateDiagram-v2
    [*] --> Uninitialized: New()
    Uninitialized --> Defaulted: Default()
    Defaulted --> Configuring: SetCommand()
    Configuring --> Building: Build()
    Building --> Built: ABuild() успешно
    Note right of Building: Структура инициализирована
    Built --> Ready: Ready = true
    Ready --> CreatingArduino: Calculate() && UArdContr == nullptr
    CreatingArduino --> Created: UArduinoControl создан
    Created --> Ready: Подключение установлено
    Ready --> SendingCommand: Calculate() && SendCommandFlag
    SendingCommand --> CommandSent: Команда отправлена
    Note right of SendingCommand: InputCommand = Command<br/>SendCommandFlag = false
    CommandSent --> Ready: Команда обработана
    Ready --> GettingSpeed: Calculate() && GetSpeed
    GettingSpeed --> SpeedReceived: Скорость получена
    Note right of GettingSpeed: Speed = SpeedValues.last()<br/>GetSpeed = false
    SpeedReceived --> Ready: Данные обновлены
    Ready --> Resetting: Reset()
    Resetting --> Ready: AReset() завершен
```

**Состояния:**
- **Uninitialized** — создан, но не инициализирован
- **Defaulted** — параметры установлены по умолчанию
- **Configuring** — настройка параметров (команда управления)
- **Building** — выполняется сборка структуры
- **Built** — структура построена
- **Ready** — готов к выполнению расчетов
- **CreatingArduino** — создается компонент `UArduinoControl`
- **Created** — компонент `UArduinoControl` создан
- **SendingCommand** — отправка команды на Arduino
- **CommandSent** — команда отправлена
- **GettingSpeed** — получение скорости двигателя
- **SpeedReceived** — скорость получена
- **Resetting** — выполняется сброс состояний

### UML-диаграмма активности

```mermaid
flowchart TD
    Start([Start ACalculate]) --> CheckArduino{UArdContr == nullptr?}
    CheckArduino -->|Да| CreateArduino[Создать UArduinoControl]
    CreateArduino --> CheckSendFlag
    CheckArduino -->|Нет| CheckSendFlag{SendCommandFlag?}
    CheckSendFlag -->|Да| SetInputCommand[UArdContr->InputCommand = Command]
    SetInputCommand --> SetSentCommand[SentCommand = Command]
    SetSentCommand --> ClearFlag[SendCommandFlag = false]
    ClearFlag --> CheckGetSpeed
    CheckSendFlag -->|Нет| CheckGetSpeed{GetSpeed?}
    CheckGetSpeed -->|Да| CheckSpeedValues["SpeedValues<br/>не пуст?"]
    CheckSpeedValues -->|Да| GetLastSpeed[Speed = SpeedValues.last()]
    CheckSpeedValues -->|Нет| ClearGetSpeed
    GetLastSpeed --> ClearGetSpeed[GetSpeed = false]
    CheckGetSpeed -->|Нет| End
    ClearGetSpeed --> End([End])
```

**Алгоритм работы ACalculate():**
1. Проверка наличия `UArduinoControl`, создание при необходимости
2. Если `SendCommandFlag == true`:
   - Установка `UArdContr->InputCommand = Command`
   - Сохранение команды в `SentCommand`
   - Сброс `SendCommandFlag = false`
3. Если `GetSpeed == true`:
   - Проверка наличия данных в `UArdContr->SpeedValues`
   - Если данные есть: установка `Speed = SpeedValues.last()`
   - Сброс `GetSpeed = false`

### UML-диаграмма компонентов

```mermaid
graph TB
    subgraph RdkHardwareLib["Rdk-HardwareLib"]
        DcControl[UDcControlDemo]
        ArduinoControl[UArduinoControl]
    end
    
    subgraph RdkBasicLib["Rdk-BasicLib"]
        UNet[UNet]
        UStorage[UStorage]
    end
    
    DcControl -->|наследуется| UNet
    DcControl -->|использует| ArduinoControl
    UStorage -->|управляет| DcControl
    
    subgraph Interfaces["Интерфейсы"]
        InputProps["Входные свойства<br/>ptInput"]
        OutputProps["Выходные свойства<br/>ptOutput"]
        Parameters["Параметры<br/>ptPubParameter"]
        States["Состояния<br/>ptPubState"]
    end
    
    DcControl --> Parameters
    DcControl --> States
    DcControl --> OutputProps
```

**Зависимости:**
- **Rdk-BasicLib** — базовые классы (`UNet`, `UStorage`, `UProperty`)
- **UArduinoControl** — компонент управления Arduino (для отправки команд и получения данных)

**Интерфейсы:**
- **Входы** — свойства с флагом `ptInput`: отсутствуют
- **Выходы** — свойства с флагом `ptOutput`: `Command`
- **Параметры** — свойства с флагом `ptPubParameter`: `Command`
- **Состояния** — свойства с флагом `ptPubState`: `SendCommandFlag`, `SentCommand`, `Speed`, `Acceleration`, `GetSpeed`

### Свойства

#### Параметры (ptPubParameter)

- **`Command`** (string, ptPubParameter | ptOutput) — команда для управления DC-двигателем. Может быть установлена как параметр или использована как выходное свойство для связи с другими компонентами. Команда отправляется на Arduino через `UArduinoControl->InputCommand` при установке `SendCommandFlag = true`. Формат команды зависит от протокола Arduino (например, "SET_SPEED 50", "SET_DIRECTION FORWARD"). Значение по умолчанию: пустая строка.

#### Состояния (ptPubState)

- **`SendCommandFlag`** (bool) — флаг для отправки команды из свойства `Command`. При установке в `true` в `ACalculate()` команда передается в `UArduinoControl->InputCommand`, после чего флаг сбрасывается в `false`. Значение по умолчанию: `false`.

- **`SentCommand`** (string) — последняя отправленная команда. Обновляется после установки команды в `UArduinoControl->InputCommand`. Значение по умолчанию: пустая строка.

- **`Speed`** (float) — текущая скорость двигателя. Обновляется из `UArduinoControl->SpeedValues.last()` при установке `GetSpeed = true`. Значение по умолчанию: 0.0. Диапазон: зависит от протокола Arduino и типа двигателя.

- **`Acceleration`** (float) — текущее ускорение двигателя. В текущей реализации не обновляется автоматически. Значение по умолчанию: 0.0.

- **`GetSpeed`** (bool) — флаг для получения скорости двигателя. При установке в `true` в `ACalculate()` скорость читается из `UArduinoControl->SpeedValues`, после чего флаг сбрасывается в `false`. Значение по умолчанию: `false`.

#### Защищенные поля

- **`UArdContr`** (UEPtr<UArduinoControl>) — указатель на объект управления Arduino. Создается автоматически в `ACalculate()` при первом вызове, если равен `nullptr`. Освобождается в `AUnInit()`.

### Методы

#### Публичные методы

- **`New()`** → `UDcControlDemo*` — создает новый экземпляр класса. Используется системой `UStorage` для создания компонентов.

#### Защищенные методы жизненного цикла

- **`ADefault()`** → `bool` — инициализирует параметры по умолчанию. В текущей реализации всегда возвращает `true`. Вызывается автоматически при `Default()`.

- **`ABuild()`** → `bool` — строит внутреннюю структуру компонента. В текущей реализации всегда возвращает `true`. Вызывается автоматически при `Build()`. Возвращает `true` при успешной сборке.

- **`AReset()`** → `bool` — сбрасывает состояние компонента. В текущей реализации всегда возвращает `true`. Вызывается автоматически при `Reset()`. Возвращает `true`.

- **`ACalculate()`** → `bool` — выполняет расчет компонента на каждом шаге. Проверяет и создает `UArduinoControl` при необходимости, обрабатывает команды и получает данные о скорости. Вызывается автоматически при `Calculate()`. Возвращает `true`.

#### Защищенные методы инициализации

- **`AInit()`** → `void` — инициализация компонента. В текущей реализации пустой. Вызывается автоматически при `Init()`.

- **`AUnInit()`** → `void` — деинициализация компонента. В текущей реализации пустой. Вызывается автоматически при `UnInit()`.

### Примеры использования в C++

#### Пример 1: Создание и настройка компонента

```cpp
// Создание компонента управления DC-двигателем
auto dcControl = storage->CreateComponent<UDcControlDemo>();
dcControl->SetName("DCMotor1");

// Инициализация
dcControl->Default();

// Настройка параметров
dcControl->Command = "SET_SPEED 50";

// Сборка
dcControl->Build();

// Использование
for (int step = 0; step < 1000; step++) {
    // Установка команды для отправки
    dcControl->Command = "SET_SPEED " + std::to_string(step % 100);
    dcControl->SendCommandFlag = true;
    
    // Получение скорости
    dcControl->GetSpeed = true;
    
    // Выполнение расчета
    dcControl->Calculate();
    
    // Получение данных
    float speed = dcControl->Speed;
    std::string sentCommand = dcControl->SentCommand;
    
    std::cout << "Step " << step << ": Command=" << sentCommand 
              << ", Speed=" << speed << std::endl;
}
```

#### Пример 2: Использование выходного соединения

```cpp
// Создание компонента-источника команды
auto commandSource = storage->CreateComponent<UScalarSource>();
commandSource->SetName("CommandSource");

// Создание компонента управления DC-двигателем
auto dcControl = storage->CreateComponent<UDcControlDemo>();
dcControl->SetName("DCMotor1");
dcControl->Build();

// Создание связи
storage->CreateLink(commandSource->Output, dcControl->Command);

// В цикле расчетов команды будут автоматически передаваться
// из CommandSource в DCMotor через Command
// Для отправки команды нужно установить SendCommandFlag = true
```

#### Пример 3: Управление скоростью с обратной связью

```cpp
auto dcControl = storage->CreateComponent<UDcControlDemo>();
dcControl->SetName("DCMotor1");
dcControl->Build();

// Целевая скорость
float targetSpeed = 75.0f;

for (int step = 0; step < 1000; step++) {
    // Получение текущей скорости
    dcControl->GetSpeed = true;
    dcControl->Calculate();
    float currentSpeed = dcControl->Speed;
    
    // ПИД-регулятор (упрощенный)
    float error = targetSpeed - currentSpeed;
    float controlSignal = error * 0.1f;  // Простое пропорциональное управление
    
    // Установка новой команды
    dcControl->Command = "SET_SPEED " + std::to_string(static_cast<int>(controlSignal));
    dcControl->SendCommandFlag = true;
    
    // Выполнение расчета
    dcControl->Calculate();
    
    std::cout << "Step " << step << ": Target=" << targetSpeed 
              << ", Current=" << currentSpeed 
              << ", Error=" << error << std::endl;
}
```

### Примеры конфигурации XML

#### Пример 1: Базовая конфигурация

```xml
<Model Class="NModel">
    <Components>
        <ArduinoControl1 Class="Arduino">
            <Parameters>
                <PortToConnect>COM3</PortToConnect>
                <MatrixCols>100</MatrixCols>
            </Parameters>
        </ArduinoControl1>
        
        <DCMotor1 Class="DC">
            <Parameters>
                <Command>SET_SPEED 50</Command>
            </Parameters>
        </DCMotor1>
    </Components>
</Model>
```

#### Пример 2: Конфигурация с входным соединением

```xml
<Model Class="NModel">
    <Components>
        <ArduinoControl1 Class="Arduino">
            <Parameters>
                <PortToConnect>/dev/ttyUSB0</PortToConnect>
                <MatrixCols>50</MatrixCols>
            </Parameters>
        </ArduinoControl1>
        
        <CommandSource Class="UScalarSource">
            <Parameters>
                <Value>"SET_SPEED 75"</Value>
            </Parameters>
        </CommandSource>
        
        <DCMotor1 Class="DC">
            <Parameters>
                <SendCommandFlag>true</SendCommandFlag>
            </Parameters>
        </DCMotor1>
    </Components>
    
    <Links>
        <elem>
            <Item>CommandSource.Output</Item>
            <Connector>DCMotor1.Command</Connector>
        </elem>
    </Links>
</Model>
```

#### Пример 3: Конфигурация с мониторингом скорости

```xml
<Model Class="NModel">
    <Components>
        <ArduinoControl1 Class="Arduino">
            <Parameters>
                <PortToConnect>COM3</PortToConnect>
                <MatrixCols>200</MatrixCols>
                <GetDataFromBuffers>true</GetDataFromBuffers>
            </Parameters>
        </ArduinoControl1>
        
        <DCMotor1 Class="DC">
            <Parameters>
                <Command>SET_SPEED 60</Command>
                <SendCommandFlag>true</SendCommandFlag>
                <GetSpeed>true</GetSpeed>
            </Parameters>
        </DCMotor1>
        
        <SpeedMonitor Class="UStatisticDoubleMatrix">
            <Parameters>
                <!-- Параметры мониторинга скорости -->
            </Parameters>
        </SpeedMonitor>
    </Components>
    
    <Links>
        <elem>
            <Item>DCMotor1.Speed</Item>
            <Connector>SpeedMonitor.Input</Connector>
        </elem>
    </Links>
</Model>
```

### Использование в конфигурациях

`UDcControlDemo` используется для управления DC-двигателями через Arduino в конфигурационных проектах. Типичные сценарии использования:

1. **Управление скоростью** — отправка команд для установки скорости двигателя через `Command` и `SendCommandFlag`
2. **Управление направлением** — отправка команд для изменения направления вращения
3. **Обратная связь** — получение данных о текущей скорости через `GetSpeed` и `Speed`
4. **Интеграция с контроллерами** — использование `Command` как выходного свойства для связи с другими компонентами

**Типичные значения параметров:**
- **Command**: "SET_SPEED 50", "SET_DIRECTION FORWARD", "SET_DIRECTION REVERSE", "STOP" (зависит от протокола Arduino)
- **SendCommandFlag**: `true` для отправки команды, `false` после отправки
- **GetSpeed**: `true` для получения скорости, `false` после получения

**Формат команд (примерный, зависит от протокола Arduino):**
- `SET_SPEED <value>` — установка скорости (0-100 или 0-255)
- `SET_DIRECTION <FORWARD|REVERSE>` — установка направления
- `STOP` — остановка двигателя
- `BRAKE` — торможение двигателя

### См. также

- [`UArduinoControl`](Arduino.md) — компонент управления Arduino
- [`UADC`](ADC.md) — компонент для работы с аналоговыми датчиками
- [Architecture.md](../Architecture.md) — архитектура библиотеки

---

## EN

### Purpose

**Class**: `UDcControlDemo` — demonstration component for DC motor control via Arduino.  
**Registration**: `UHardwareLibrary.cpp` → `UploadClass("DC", ...)`.  
**Storage instances**: `ClassName = "DC"` in `Bin/Configs/*/Model_*.xml`.

`UDcControlDemo` extends `UNet` with DC motor control functionality. Allows sending motor control commands (speed, direction), receiving speed and acceleration feedback. Uses `UArduinoControl` to send commands to Arduino.

### UML Class Diagram

```mermaid
classDiagram
    UNet <|-- UDcControlDemo
    UDcControlDemo *-- UArduinoControl : UArdContr
    class UNet {
        +ADefault() bool
        +ABuild() bool
        +AReset() bool
        +ACalculate() bool
        +AInit() void
        +AUnInit() void
    }
    class UDcControlDemo {
        +Command : string
        +SendCommandFlag : bool
        +SentCommand : string
        +Speed : float
        +Acceleration : float
        +GetSpeed : bool
        -UArdContr : UEPtr~UArduinoControl~
        +New() UDcControlDemo*
        #ADefault() bool
        #ABuild() bool
        #AReset() bool
        #ACalculate() bool
        #AInit() void
        #AUnInit() void
    }
    class UArduinoControl {
        +InputCommand : string
        +SpeedValues : QVector~double~
    }
```

**Inheritance hierarchy:**
- `UNet` — base network component class from Rdk-BasicLib
- `UDcControlDemo` — DC motor control component

**Relationships with other components:**
- `UDcControlDemo` uses `UArduinoControl` (composition via `UEPtr`) to send commands to Arduino
- `UArduinoControl` provides speed data via `SpeedValues`

### UML Sequence Diagram

```mermaid
sequenceDiagram
    participant Storage as UStorage
    participant DC as UDcControlDemo
    participant Arduino as UArduinoControl
    participant Connect as UArduinoConnect
    
    Storage->>DC: New()
    Storage->>DC: Default()
    DC->>DC: ADefault()
    Note over DC: Initialize default<br/>parameters
    Storage->>DC: SetCommand("SET_SPEED 50")
    Storage->>DC: Build()
    DC->>DC: ABuild()
    Note over DC: Initialize structure
    DC-->>Storage: Ready = true
    loop Each calculation step
        Storage->>DC: Calculate()
        DC->>DC: ACalculate()
        alt UArdContr == nullptr
            DC->>Arduino: new UArduinoControl()
            Note over Arduino: Create Arduino<br/>control component
        end
        alt SendCommandFlag == true
            DC->>Arduino: InputCommand = Command
            DC->>DC: SentCommand = Command
            DC->>DC: SendCommandFlag = false
            Arduino->>Connect: Send command to Arduino
        end
        alt GetSpeed == true
            DC->>Arduino: Check SpeedValues
            alt SpeedValues not empty
                Arduino-->>DC: SpeedValues.last()
                DC->>DC: Speed = SpeedValues.last()
            end
            DC->>DC: GetSpeed = false
        end
    end
    Storage->>DC: Reset()
    DC->>DC: AReset()
    Note over DC: Reset state
```

**Lifecycle:**
1. **Creation** — component is created via constructor
2. **Initialization** — default parameters set via `ADefault()`
3. **Configuration** — control command set via `Command`
4. **Build** — structure initialized via `ABuild()`
5. **Calculation** — on each step:
   - Check and create `UArduinoControl` if necessary
   - Send command via `InputCommand` (if `SendCommandFlag == true`)
   - Obtain speed from `UArduinoControl->SpeedValues` (if `GetSpeed == true`)
6. **Reset** — state reset via `AReset()`

### UML State Diagram

```mermaid
stateDiagram-v2
    [*] --> Uninitialized: New()
    Uninitialized --> Defaulted: Default()
    Defaulted --> Configuring: SetCommand()
    Configuring --> Building: Build()
    Building --> Built: ABuild() success
    Note right of Building: Structure initialized
    Built --> Ready: Ready = true
    Ready --> CreatingArduino: Calculate() && UArdContr == nullptr
    CreatingArduino --> Created: UArduinoControl created
    Created --> Ready: Connection established
    Ready --> SendingCommand: Calculate() && SendCommandFlag
    SendingCommand --> CommandSent: Command sent
    Note right of SendingCommand: InputCommand = Command<br/>SendCommandFlag = false
    CommandSent --> Ready: Command processed
    Ready --> GettingSpeed: Calculate() && GetSpeed
    GettingSpeed --> SpeedReceived: Speed received
    Note right of GettingSpeed: Speed = SpeedValues.last()<br/>GetSpeed = false
    SpeedReceived --> Ready: Data updated
    Ready --> Resetting: Reset()
    Resetting --> Ready: AReset() complete
```

**States:**
- **Uninitialized** — created but not initialized
- **Defaulted** — default parameters set
- **Configuring** — configuring parameters (control command)
- **Building** — structure build in progress
- **Built** — structure built
- **Ready** — ready for calculations
- **CreatingArduino** — creating `UArduinoControl` component
- **Created** — `UArduinoControl` component created
- **SendingCommand** — sending command to Arduino
- **CommandSent** — command sent
- **GettingSpeed** — obtaining motor speed
- **SpeedReceived** — speed received
- **Resetting** — state reset in progress

### UML Activity Diagram

```mermaid
flowchart TD
    Start([Start ACalculate]) --> CheckArduino{UArdContr == nullptr?}
    CheckArduino -->|Yes| CreateArduino[Create UArduinoControl]
    CreateArduino --> CheckSendFlag
    CheckArduino -->|No| CheckSendFlag{SendCommandFlag?}
    CheckSendFlag -->|Yes| SetInputCommand[UArdContr->InputCommand = Command]
    SetInputCommand --> SetSentCommand[SentCommand = Command]
    SetSentCommand --> ClearFlag[SendCommandFlag = false]
    ClearFlag --> CheckGetSpeed
    CheckSendFlag -->|No| CheckGetSpeed{GetSpeed?}
    CheckGetSpeed -->|Yes| CheckSpeedValues["SpeedValues<br/>not empty?"]
    CheckSpeedValues -->|Yes| GetLastSpeed[Speed = SpeedValues.last()]
    CheckSpeedValues -->|No| ClearGetSpeed
    GetLastSpeed --> ClearGetSpeed[GetSpeed = false]
    CheckGetSpeed -->|No| End
    ClearGetSpeed --> End([End])
```

**ACalculate() algorithm:**
1. Check for `UArduinoControl`, create if necessary
2. If `SendCommandFlag == true`:
   - Set `UArdContr->InputCommand = Command`
   - Save command in `SentCommand`
   - Reset `SendCommandFlag = false`
3. If `GetSpeed == true`:
   - Check for data in `UArdContr->SpeedValues`
   - If data exists: set `Speed = SpeedValues.last()`
   - Reset `GetSpeed = false`

### UML Component Diagram

```mermaid
graph TB
    subgraph RdkHardwareLib["Rdk-HardwareLib"]
        DcControl[UDcControlDemo]
        ArduinoControl[UArduinoControl]
    end
    
    subgraph RdkBasicLib["Rdk-BasicLib"]
        UNet[UNet]
        UStorage[UStorage]
    end
    
    DcControl -->|inherits| UNet
    DcControl -->|uses| ArduinoControl
    UStorage -->|manages| DcControl
    
    subgraph Interfaces["Interfaces"]
        InputProps["Input properties<br/>ptInput"]
        OutputProps["Output properties<br/>ptOutput"]
        Parameters["Parameters<br/>ptPubParameter"]
        States["States<br/>ptPubState"]
    end
    
    DcControl --> Parameters
    DcControl --> States
    DcControl --> OutputProps
```

**Dependencies:**
- **Rdk-BasicLib** — base classes (`UNet`, `UStorage`, `UProperty`)
- **UArduinoControl** — Arduino control component (for sending commands and obtaining data)

**Interfaces:**
- **Inputs** — properties with `ptInput` flag: none
- **Outputs** — properties with `ptOutput` flag: `Command`
- **Parameters** — properties with `ptPubParameter` flag: `Command`
- **States** — properties with `ptPubState` flag: `SendCommandFlag`, `SentCommand`, `Speed`, `Acceleration`, `GetSpeed`

### Properties

#### Parameters (ptPubParameter)

- **`Command`** (string, ptPubParameter | ptOutput) — command for DC motor control. Can be set as a parameter or used as an output property for connection to other components. Command is sent to Arduino via `UArduinoControl->InputCommand` when `SendCommandFlag = true`. Command format depends on Arduino protocol (e.g., "SET_SPEED 50", "SET_DIRECTION FORWARD"). Default: empty string.

#### States (ptPubState)

- **`SendCommandFlag`** (bool) — flag to send command from `Command` property. When set to `true` in `ACalculate()`, command is passed to `UArduinoControl->InputCommand`, then flag is reset to `false`. Default: `false`.

- **`SentCommand`** (string) — last sent command. Updated after setting command in `UArduinoControl->InputCommand`. Default: empty string.

- **`Speed`** (float) — current motor speed. Updated from `UArduinoControl->SpeedValues.last()` when `GetSpeed = true`. Default: 0.0. Range: depends on Arduino protocol and motor type.

- **`Acceleration`** (float) — current motor acceleration. Not updated automatically in current implementation. Default: 0.0.

- **`GetSpeed`** (bool) — flag to obtain motor speed. When set to `true` in `ACalculate()`, speed is read from `UArduinoControl->SpeedValues`, then flag is reset to `false`. Default: `false`.

#### Protected fields

- **`UArdContr`** (UEPtr<UArduinoControl>) — pointer to Arduino control object. Created automatically in `ACalculate()` on first call if equal to `nullptr`. Released in `AUnInit()`.

### Methods

#### Public methods

- **`New()`** → `UDcControlDemo*` — creates a new class instance. Used by the `UStorage` system to create components.

#### Protected lifecycle methods

- **`ADefault()`** → `bool` — initializes default parameters. Always returns `true` in current implementation. Called automatically on `Default()`.

- **`ABuild()`** → `bool` — builds internal component structure. Always returns `true` in current implementation. Called automatically on `Build()`. Returns `true` on successful build.

- **`AReset()`** → `bool` — resets component state. Always returns `true` in current implementation. Called automatically on `Reset()`. Returns `true`.

- **`ACalculate()`** → `bool` — performs component calculation on each step. Checks and creates `UArduinoControl` if necessary, processes commands and obtains speed data. Called automatically on `Calculate()`. Returns `true`.

#### Protected initialization methods

- **`AInit()`** → `void` — component initialization. Empty in current implementation. Called automatically on `Init()`.

- **`AUnInit()`** → `void` — component deinitialization. Empty in current implementation. Called automatically on `UnInit()`.

### Usage Examples in C++

#### Example 1: Creating and configuring the component

```cpp
// Create DC motor control component
auto dcControl = storage->CreateComponent<UDcControlDemo>();
dcControl->SetName("DCMotor1");

// Initialization
dcControl->Default();

// Configure parameters
dcControl->Command = "SET_SPEED 50";

// Build
dcControl->Build();

// Usage
for (int step = 0; step < 1000; step++) {
    // Set command for sending
    dcControl->Command = "SET_SPEED " + std::to_string(step % 100);
    dcControl->SendCommandFlag = true;
    
    // Get speed
    dcControl->GetSpeed = true;
    
    // Perform calculation
    dcControl->Calculate();
    
    // Get data
    float speed = dcControl->Speed;
    std::string sentCommand = dcControl->SentCommand;
    
    std::cout << "Step " << step << ": Command=" << sentCommand 
              << ", Speed=" << speed << std::endl;
}
```

#### Example 2: Using output connection

```cpp
// Create command source component
auto commandSource = storage->CreateComponent<UScalarSource>();
commandSource->SetName("CommandSource");

// Create DC motor control component
auto dcControl = storage->CreateComponent<UDcControlDemo>();
dcControl->SetName("DCMotor1");
dcControl->Build();

// Create link
storage->CreateLink(commandSource->Output, dcControl->Command);

// In the calculation loop commands are automatically transferred
// from CommandSource to DCMotor via Command
// To send command set SendCommandFlag = true
```

#### Example 3: Speed control with feedback

```cpp
auto dcControl = storage->CreateComponent<UDcControlDemo>();
dcControl->SetName("DCMotor1");
dcControl->Build();

// Target speed
float targetSpeed = 75.0f;

for (int step = 0; step < 1000; step++) {
    // Get current speed
    dcControl->GetSpeed = true;
    dcControl->Calculate();
    float currentSpeed = dcControl->Speed;
    
    // PID controller (simplified)
    float error = targetSpeed - currentSpeed;
    float controlSignal = error * 0.1f;  // Simple proportional control
    
    // Set new command
    dcControl->Command = "SET_SPEED " + std::to_string(static_cast<int>(controlSignal));
    dcControl->SendCommandFlag = true;
    
    // Perform calculation
    dcControl->Calculate();
    
    std::cout << "Step " << step << ": Target=" << targetSpeed 
              << ", Current=" << currentSpeed 
              << ", Error=" << error << std::endl;
}
```

### XML Configuration Examples

#### Example 1: Basic configuration

```xml
<Model Class="NModel">
    <Components>
        <ArduinoControl1 Class="Arduino">
            <Parameters>
                <PortToConnect>COM3</PortToConnect>
                <MatrixCols>100</MatrixCols>
            </Parameters>
        </ArduinoControl1>
        
        <DCMotor1 Class="DC">
            <Parameters>
                <Command>SET_SPEED 50</Command>
            </Parameters>
        </DCMotor1>
    </Components>
</Model>
```

#### Example 2: Configuration with input connection

```xml
<Model Class="NModel">
    <Components>
        <ArduinoControl1 Class="Arduino">
            <Parameters>
                <PortToConnect>/dev/ttyUSB0</PortToConnect>
                <MatrixCols>50</MatrixCols>
            </Parameters>
        </ArduinoControl1>
        
        <CommandSource Class="UScalarSource">
            <Parameters>
                <Value>"SET_SPEED 75"</Value>
            </Parameters>
        </CommandSource>
        
        <DCMotor1 Class="DC">
            <Parameters>
                <SendCommandFlag>true</SendCommandFlag>
            </Parameters>
        </DCMotor1>
    </Components>
    
    <Links>
        <elem>
            <Item>CommandSource.Output</Item>
            <Connector>DCMotor1.Command</Connector>
        </elem>
    </Links>
</Model>
```

#### Example 3: Configuration with speed monitoring

```xml
<Model Class="NModel">
    <Components>
        <ArduinoControl1 Class="Arduino">
            <Parameters>
                <PortToConnect>COM3</PortToConnect>
                <MatrixCols>200</MatrixCols>
                <GetDataFromBuffers>true</GetDataFromBuffers>
            </Parameters>
        </ArduinoControl1>
        
        <DCMotor1 Class="DC">
            <Parameters>
                <Command>SET_SPEED 60</Command>
                <SendCommandFlag>true</SendCommandFlag>
                <GetSpeed>true</GetSpeed>
            </Parameters>
        </DCMotor1>
        
        <SpeedMonitor Class="UStatisticDoubleMatrix">
            <Parameters>
                <!-- Speed monitoring parameters -->
            </Parameters>
        </SpeedMonitor>
    </Components>
    
    <Links>
        <elem>
            <Item>DCMotor1.Speed</Item>
            <Connector>SpeedMonitor.Input</Connector>
        </elem>
    </Links>
</Model>
```

### Usage in Configurations

`UDcControlDemo` is used to control DC motors via Arduino in configuration projects. Typical use cases:

1. **Speed control** — sending commands to set motor speed via `Command` and `SendCommandFlag`
2. **Direction control** — sending commands to change rotation direction
3. **Feedback** — obtaining current speed data via `GetSpeed` and `Speed`
4. **Controller integration** — using `Command` as output property for connection to other components

**Typical parameter values:**
- **Command**: "SET_SPEED 50", "SET_DIRECTION FORWARD", "SET_DIRECTION REVERSE", "STOP" (depends on Arduino protocol)
- **SendCommandFlag**: `true` to send command, `false` after sending
- **GetSpeed**: `true` to obtain speed, `false` after obtaining

**Command format (example, depends on Arduino protocol):**
- `SET_SPEED <value>` — set speed (0-100 or 0-255)
- `SET_DIRECTION <FORWARD|REVERSE>` — set direction
- `STOP` — stop motor
- `BRAKE` — brake motor

### See Also

- [`UArduinoControl`](Arduino.md) — Arduino control component
- [`UADC`](ADC.md) — analog sensor component
- [Architecture.md](../Architecture.md) — library architecture
