# Обзор API Rdk-HardwareLib

Краткий справочник свойств. Источник правды — заголовки в `Libraries/Rdk-HardwareLib/Core/`.

**Edge** — `bool` вход: `true` на тик → действие → сброс в `false`. **State** — только чтение схемой/GUI.

## ConnectionState (`UArduinoBoard`)

| Значение | Смысл |
|----------|--------|
| 0 | Disconnected |
| 1 | Opening |
| 2 | Connected |
| 3 | Error |

## ArduinoBoard (`UArduinoBoard`)

| Свойство | Тип | Роль |
|----------|-----|------|
| `PortName` | string | `/dev/ttyACM0`, `COM3`, … |
| `BaudRate` | int | По умолчанию **57600** |
| `BoardProfile` | int | 0 = Uno, 1 = Mega 2560 |
| `AutoReconnect` | bool | Retry при ошибке |
| `ConnectOnBuild` | bool | `ABuild` → connect |
| `Connect` | bool | **edge** — открыть порт |
| `Disconnect` | bool | **edge** — закрыть (не очищает `PortName`) |
| `Reconnect` | bool | **edge** — close + open |
| `UploadFirmware` | bool | **edge** — прошивка |
| `ClearLastError` | bool | **edge** |
| `ConnectionState` | int | state |
| `LastError` | string | state |
| `LastActivityMs` | double | state |
| `IsConnected` | bool | state |
| `IsOpening` | bool | state |
| `HasError` | bool | state |
| `IsDisconnected` | bool | state |
| `IsUploading` | bool | state |
| `UploadComplete` | bool | state |
| `HeartbeatEnabled` | bool | param |
| `HeartbeatIntervalMs` | int | param |
| `HeartbeatTimeoutMs` | int | param |
| `MissedHeartbeats` | int | state |
| `RequestHealthCheck` | bool | edge (legacy) |
| `FirmwarePath` | string | param |
| `BundledFirmwareId` | string | `sensor_lab_v1`, `standard_firmata` |
| `UploadFirmwareFlag` | bool | edge (legacy ≡ `UploadFirmware`) |
| `UploadProgress` | int | state 0–100 |
| `UploadLastResult` | string | state |
| `ShowDebug` | bool | param |

## ArduinoCustomLink (база, не в палитре)

Наследует `ArduinoBoard` + :

| Свойство | Тип | Роль |
|----------|-----|------|
| `Command` | string | param |
| `SendCommand` | bool | **edge** + `Command` |
| `RequestGetStatus` | bool | **edge** |
| `RequestProtocolNegotiate` | bool | **edge** |
| `SendCommandFlag` | bool | legacy edge |
| `SentCommand` | string | state |
| `InputCommand` | string | input (IsNewData) |
| `ProtocolVersion` | int | 1 = legacy, 2 = framed |
| `RxFrameCount` | int | state |
| `TxCommandCount` | int | state |
| `IsProtocolReady` | bool | state |
| `HasPendingCommands` | bool | state |
| `LastSentCommand` | string | state |

## ArduinoSensorSketch (`UArduinoSensorSketch`)

Плюс CustomLink + Board:

| Свойство | Тип | Роль |
|----------|-----|------|
| `LowerSensorLimit` / `UpperSensorLimit` | double | param |
| `MatrixCols` | int | param |
| `GetDataFromBuffers` | bool | edge |
| `GetPinsInfo` | bool | edge |
| `StartReading` / `StopReading` | bool | edge |
| `Rotate` / `StopRotate` | bool | edge |
| `DoubleMatrixReadings` | MDMatrix | state |
| `PinStatusJson` | string | state |

## ArduinoFirmata (`UArduinoFirmata`)

| Свойство | Тип | Роль |
|----------|-----|------|
| `RestartFirmata` | bool | **edge** |
| `ApplyPinConfig` | bool | **edge** |
| `FirmataReady` | bool | state (legacy) |
| `IsFirmataReady` | bool | state |
| `IsLinkReady` | bool | state (= connected ∧ firmata ready) |
| `FirmataFirmwareVersion` | string | state |
| `SelectedPin` / `SelectedPinMode` | int | param |
| `DigitalPinValue` / `AnalogPinValue` | int | param/state |
| `SetPinModeFlag` / `WriteDigitalFlag` / `ReadAnalogFlag` | bool | legacy edge |
| `ReportAnalogEnable` | bool | param |

Плюс все свойства `ArduinoBoard`. Default `BundledFirmwareId` = `standard_firmata`.

## ArduinoAdc (`UArduinoAdc`)

| Свойство | Тип | Роль |
|----------|-----|------|
| `LinkedFirmataName` | string | Имя `ArduinoFirmata` на canvas |
| `AnalogPin` | int | Firmata pin # |
| `AdcValue` | int | state |
| `AdcReadOk` | bool | state |
| `ReadAdcFlag` | bool | edge |

Не открывает serial — только через Firmata.

## ArduinoDcDemo (`UArduinoDcDemo`)

Наследует **ArduinoCustomLink** (один узел на canvas):

| Свойство | Тип | Роль |
|----------|-----|------|
| `Command` | string | param |
| `SendCommand` | bool | **edge** |
| `GetSpeed` | bool | **edge** |
| `Speed` / `Acceleration` | float | state |
| `LinkedSketchName` | string | **deprecated** — delegate на sketch |

Default `BundledFirmwareId` = `sensor_lab_v1`.

## Вспомогательные классы

| Класс | Назначение |
|-------|------------|
| `UArduinoSerialSession` | QSerialPort wrapper |
| `UArduinoFlasher` | avrdude via QProcess |
| `UArduinoBoardProfile` | Uno/Mega avrdude args |
| `UArduinoBinaryStreamParser` | RX framing |
| `UArduinoFirmataClient` | Firmata MVP |
| `UFirmwareManifest` | Resolve bundled hex |

Подробнее: [Transport.md](Transport.md), [Protocol.md](Protocol.md).

## GUI form id

| ClassName | `componentGuiId` |
|-----------|-------------------|
| `ArduinoBoard` | `hw.arduino.board` |
| `ArduinoSensorSketch` | `hw.arduino.sensor_sketch` |
| `ArduinoFirmata` | `hw.arduino.firmata` |
| `ArduinoDcDemo` | `hw.arduino.dc_demo` |
| `ArduinoAdc` | `hw.arduino.adc` |

## См. также

- [Components/](Components/) — страницы по каждому ClassName
- [Usage-Examples.md](Usage-Examples.md)
- [Architecture.md](Architecture.md) — threading
