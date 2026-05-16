# Обзор API Rdk-HardwareLib

Краткий справочник свойств. Источник правды — заголовки в `Libraries/Rdk-HardwareLib/Core/`.

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
| `ConnectionState` | int | readonly |
| `LastError` | string | readonly |
| `LastActivityMs` | double | readonly |
| `HeartbeatEnabled` | bool | |
| `HeartbeatIntervalMs` | int | |
| `HeartbeatTimeoutMs` | int | |
| `MissedHeartbeats` | int | readonly |
| `RequestHealthCheck` | bool | edge |
| `FirmwarePath` | string | Абсолютный путь к HEX |
| `BundledFirmwareId` | string | `sensor_lab_v1`, `standard_firmata` |
| `UploadFirmwareFlag` | bool | edge |
| `UploadProgress` | int | 0–100 |
| `UploadLastResult` | string | `ok` или лог avrdude |
| `ShowDebug` | bool | Лог serial/parser |

## ArduinoCustomLink (база sketch, не в палитре)

Наследует `ArduinoBoard` + :

| Свойство | Тип |
|----------|-----|
| `Command` | string |
| `SendCommandFlag` | bool |
| `SentCommand` | string |
| `InputCommand` | string |
| `ProtocolVersion` | int | 1 = legacy, 2 = framed |
| `RxFrameCount` | int |
| `TxCommandCount` | int |

## ArduinoSensorSketch (`UArduinoSensorSketch`)

| Свойство | Тип |
|----------|-----|
| `LowerSensorLimit` | double |
| `UpperSensorLimit` | double |
| `MatrixCols` | int |
| `GetDataFromBuffers` | bool |
| `DoubleMatrixReadings` | MDMatrix |
| `GetPinsInfo` | bool |
| `PinStatusJson` | string |

## ArduinoFirmata (`UArduinoFirmata`)

| Свойство | Тип |
|----------|-----|
| `FirmataReady` | bool |
| `FirmataFirmwareVersion` | string |
| `SelectedPin` | int |
| `SelectedPinMode` | int |
| `DigitalPinValue` | int |
| `AnalogPinValue` | int |
| `SetPinModeFlag` | bool |
| `ReadAnalogFlag` | bool |
| `WriteDigitalFlag` | bool |

Плюс все свойства `ArduinoBoard`. Default `BundledFirmwareId` = `standard_firmata`.

## ArduinoAdc (`UArduinoAdc`)

| Свойство | Тип |
|----------|-----|
| `LinkedFirmataName` | string | Имя компонента `ArduinoFirmata` на canvas |
| `AnalogPin` | int |
| `AdcValue` | int | readonly |
| `ReadAdcFlag` | bool | edge |

Не открывает serial сам — только через Firmata.

## ArduinoDcDemo (`UArduinoDcDemo`)

| Свойство | Тип |
|----------|-----|
| `LinkedSketchName` | string | Имя `ArduinoSensorSketch` |
| `Command` | string |
| `SendCommandFlag` | bool |
| `SentCommand` | string |
| `Speed` | float |
| `Acceleration` | float |
| `GetSpeed` | bool |

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

## UHardwareLibrary

```cpp
class UHardwareLibrary : public ULibrary {
public:
    void CreateClassSamples(UStorage* storage);
};
```

## GUI

Формы: `hw.arduino.board`, `hw.arduino.sensor_sketch`, `hw.arduino.firmata` — см. [GUI.md](GUI.md).

## См. также

- [Components/](Components/) — страницы по каждому ClassName
- [Usage-Examples.md](Usage-Examples.md)
