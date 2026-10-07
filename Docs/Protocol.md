# Протокол sensor_lab

## RU

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

Хабы Nmsdk (sensor / motor / I2C) используют тот же framing — [Hub-Protocols.md](Hub-Protocols.md). Sensor hub может отдать 5-й float (`ds18b20`) при сборке с `NMSDK_SENSOR_HUB_DS18B20`.

## Firmata (отдельный транспорт)

Не через `UArduinoBinaryStreamParser`. Клиент: `UArduinoFirmataClient` (внутренний флаг `HandshakeReady` после firmware/capability/analog mapping; не путать с `UModule::Ready`).  
См. [Components/ArduinoFirmata.md](Components/ArduinoFirmata.md), [firmata_spike.md](firmata_spike.md).

## Связанные hub-протоколы

| Прошивка | Документ | Sample |
|----------|----------|--------|
| `nmsdk_sensor_hub_v1` (+ ESP32) | [Hub-Protocols.md](Hub-Protocols.md) | `14-SensorHub` |
| `nmsdk_motor_hub_v1` | [Hub-Protocols.md](Hub-Protocols.md) | `09`–`11` |
| `nmsdk_i2c_hub_v1` | [Hub-Protocols.md](Hub-Protocols.md), [Tier-CD-Modules.md](Tier-CD-Modules.md) | `15-I2cHub-BME280` |
| WaveRover JSON | [Protocol-WaveshareUgvJson.md](Protocol-WaveshareUgvJson.md) | `10-WaveRover` |

P1 sensor hub: DHT22 — `-DDHTTYPE=DHT22`; DS18B20 — `-DNMSDK_SENSOR_HUB_DS18B20=1`.

## См. также

- [Components/ArduinoSensorSketch.md](Components/ArduinoSensorSketch.md)
- [Hub-Protocols.md](Hub-Protocols.md)
- [Firmware/README.md](../Firmware/README.md)

---

## EN

Parser: `UArduinoBinaryStreamParser` (`Core/Protocol/`).  
Firmware: `Firmware/sensor_lab/sensor_lab.ino` @ **57600**.

## Text commands (host → device)

Line + `\n`. Queue in `UArduinoCustomLink::EnqueueCommand`.

### Property edge (host)

In schema/GUI use edge properties instead of direct C++ calls (reset to `false` after `ACalculate` tick):

| Property | Effect |
|----------|--------|
| `SendCommand` | Send `Command` (empty string — no-op, edge still resets) |
| `RequestGetStatus` | `GET STATUS\n` |
| `RequestProtocolNegotiate` | Reset `ProtocolNegotiated`, then `PROTO n` on connect |
| `StartReading` / `StopReading` / `Rotate` / `StopRotate` | Presets on `ArduinoSensorSketch` (set `Command` and `SendCommand`) |

See [API-Overview.md](API-Overview.md), [Architecture.md](Architecture.md) (threading).

| Command | Action |
|---------|--------|
| `START READING` | Periodic sensor data transmission |
| `STOP READING` | Stop |
| `ROTATE` / `STOP ROTATE` | Servo |
| `GET STATUS` | Status / pins request |
| `SET PINS COUNT n` | Set analog pin count (+ pin lines) |
| `PROTO 2` | Switch to framed v2 (reply `PROTO OK 2`) |

## Legacy binary (ProtocolVersion = 1)

Byte stream, packet ID in first byte:

| ID | Content |
|----|---------|
| `0x01` | Sensor frame (timestamp, floats: t, h, hall, speed) |
| `0x02` | Pin list after SET PINS COUNT |
| `0x04` | Pin status / configuration |

`UArduinoSensorSketch::OnBinaryFrame` — `0x01` → buffer/matrix, `0x04` → `PinStatusJson`.

## Framed v2 (ProtocolVersion = 2)

```
0xAA | type:u8 | len:u16 LE | payload[len] | crc8
```

CRC-8/MAXIM over bytes from `0xAA` inclusive through end of payload.

On host: `ProtocolVersion` ≥ 2 and `PROTO 2` command on connect.  
Counter `RxFrameCount` increments on successful parse.

Nmsdk hubs (sensor / motor / I2C) reuse this framing — see [Hub-Protocols.md](Hub-Protocols.md). Sensor hub may emit a 5th float (`ds18b20`) when built with `NMSDK_SENSOR_HUB_DS18B20`.

## Firmata (separate transport)

Not via `UArduinoBinaryStreamParser`. Client: `UArduinoFirmataClient` (internal `HandshakeReady` flag after firmware/capability/analog mapping; do not confuse with `UModule::Ready`).  
See [Components/ArduinoFirmata.md](Components/ArduinoFirmata.md), [firmata_spike.md](firmata_spike.md).

## Related hub protocols (not sensor_lab ClassName)

Framed v2 hubs share the same `0xAA|type|len|payload|crc8` framing. Details and commands:

| Firmware | Doc | Sample |
|----------|-----|--------|
| `nmsdk_sensor_hub_v1` (+ ESP32 twin) | [Hub-Protocols.md](Hub-Protocols.md) | `14-SensorHub` |
| `nmsdk_motor_hub_v1` | [Hub-Protocols.md](Hub-Protocols.md) | `09`–`11` wheeled |
| `nmsdk_i2c_hub_v1` | [Hub-Protocols.md](Hub-Protocols.md), [Tier-CD-Modules.md](Tier-CD-Modules.md) | `15-I2cHub-BME280` |
| WaveRover JSON | [Protocol-WaveshareUgvJson.md](Protocol-WaveshareUgvJson.md) | `10-WaveRover` |

Sensor hub extras (P1): DHT22 via `-DDHTTYPE=DHT22`; DS18B20 via `-DNMSDK_SENSOR_HUB_DS18B20=1` (5th float `ds18b20`).

## See also

- [Components/ArduinoSensorSketch.md](Components/ArduinoSensorSketch.md)
- [Hub-Protocols.md](Hub-Protocols.md)
- [Firmware/README.md](../Firmware/README.md)
