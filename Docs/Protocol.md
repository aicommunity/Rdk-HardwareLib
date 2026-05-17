# Протокол sensor_lab

Парсер: `UArduinoBinaryStreamParser` (`Core/Protocol/`).  
Прошивка: `Firmware/sensor_lab/sensor_lab.ino` @ **57600**.

## Текстовые команды (хост → устройство)

Строка + `\n`. Очередь в `UArduinoCustomLink::EnqueueCommand`.

### Property edge (хост)

В схеме/GUI вместо прямого вызова C++ используйте edge-свойства (сброс в `false` после тика `ACalculate`):

| Свойство | Эффект |
|----------|--------|
| `SendCommand` | Отправить `Command` (пустая строка — no-op, edge всё равно сбрасывается) |
| `RequestGetStatus` | `GET STATUS\n` |
| `RequestProtocolNegotiate` | Сброс `ProtocolNegotiated`, затем `PROTO n` при connect |
| `StartReading` / `StopReading` / `Rotate` / `StopRotate` | Пресеты на `ArduinoSensorSketch` (устанавливают `Command` и `SendCommand`) |

См. [API-Overview.md](API-Overview.md), [Architecture.md](Architecture.md) (threading).

| Команда | Действие |
|---------|----------|
| `START READING` | Периодическая отправка сенсорных данных |
| `STOP READING` | Останов |
| `ROTATE` / `STOP ROTATE` | Серво |
| `GET STATUS` | Запрос статуса / пинов |
| `SET PINS COUNT n` | Задать число аналоговых пинов (+ строки пинов) |
| `PROTO 2` | Переключение на framed v2 (ответ `PROTO OK 2`) |

## Legacy binary (ProtocolVersion = 1)

Поток байтов, идентификатор пакета в первом байте:

| ID | Содержимое |
|----|------------|
| `0x01` | Сенсорный кадр (timestamp, floats: t, h, hall, speed) |
| `0x02` | Список пинов после SET PINS COUNT |
| `0x04` | Статус пинов / конфигурация |

`UArduinoSensorSketch::OnBinaryFrame` — `0x01` → буфер/матрица, `0x04` → `PinStatusJson`.

## Framed v2 (ProtocolVersion = 2)

```
0xAA | type:u8 | len:u16 LE | payload[len] | crc8
```

CRC-8/MAXIM по байтам от `0xAA` включительно до конца payload.

На хосте: `ProtocolVersion` ≥ 2 и команда `PROTO 2` при connect.  
Счётчик `RxFrameCount` растёт при успешном разборе.

## Firmata (отдельный транспорт)

Не через `UArduinoBinaryStreamParser`. Клиент: `UArduinoFirmataClient` (внутренний флаг `HandshakeReady` после firmware/capability/analog mapping; не путать с `UModule::Ready`).  
См. [Components/ArduinoFirmata.md](Components/ArduinoFirmata.md), [firmata_spike.md](firmata_spike.md).

## См. также

- [Components/ArduinoSensorSketch.md](Components/ArduinoSensorSketch.md)
- [Firmware/README.md](../Firmware/README.md)
