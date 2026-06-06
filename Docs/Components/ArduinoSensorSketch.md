# ArduinoSensorSketch

## Назначение

**ClassName:** `ArduinoSensorSketch`  
**C++:** `UArduinoSensorSketch` : `UArduinoCustomLink` : `UArduinoBoard`  
**Роль:** Прошивка `sensor_lab` — текстовые команды + бинарные кадры (DHT, Hall, Servo).

## Ключевые свойства

Наследует `ArduinoBoard` и `UArduinoCustomLink`.

| Свойство | Роль |
|----------|------|
| `Command` | Текст команды |
| `SendCommand` | **Edge:** отправить `Command` |
| `RequestGetStatus` | **Edge:** `GET STATUS` |
| `StartReading` / `StopReading` | **Edge:** пресеты чтения |
| `Rotate` / `StopRotate` | **Edge:** серво |
| `GetDataFromBuffers` | **Edge:** RX `0x01` → `DoubleMatrixReadings` |
| `GetPinsInfo` | **Edge:** `GET STATUS` → `PinStatusJson` |
| `ProtocolVersion` | 1 = legacy, 2 = framed |
| `IsProtocolReady` | State |

Legacy: `SendCommandFlag` ≡ `SendCommand`.

## GUI

`hw.arduino.sensor_sketch` — вкладки **Sensor** | **Board**. Порт и upload только на вкладке Board. Отправка: `pulseEdge("SendCommand")`.

## Команды (пресеты)

`START READING`, `STOP READING`, `ROTATE`, `STOP ROTATE`, `GET STATUS`, `GET PINS INFO` — см. [Protocol.md](../Protocol.md).

## Тестовый конфиг

`Bin/Configs/SpikeSamples/Hardware/02-ArduinoSensorSketch/`

## ClDesc

`Bin/ClDesc/HardwareLibrary/ru-RU/ArduinoSensorSketch.xml`
