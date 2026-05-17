# Transport layer

Внутренние классы в `Core/Transport/` (не регистрируются в Storage).

## UArduinoSerialSession

**Файлы:** `UArduinoSerialSession.{h,cpp}`

| API | Описание |
|-----|----------|
| `open(port, baud)` | Несколько кандидатов имён (systemLocation, normalized path) |
| `close()` | |
| `isOpen()` | |
| `bytesToWrite()` | Очередь команд ждёт `== 0` перед следующей записью |
| `write(data)` | |
| `takeReceivedBytes()` | Swap RX buffer (thread-safe) |
| `lastError()` | Текст при неудачном open |
| `availablePorts()` | Список путей через `UArduinoSerialPortUtil` |

**Сигналы:** `bytesReceived`, `errorOccurred`.

**Threading:** `readyRead` только пишет в mutex-буфер. В HardwareLib **нет** `connect(bytesReceived, …)` к `UNet` — компоненты забирают данные в `UArduinoBoard::ACalculate` через `takeReceivedBytes()` (поток движка). См. [Architecture.md](Architecture.md).

**Права Linux:** при `Permission denied` в `lastError` — подсказка про группу `dialout`.

## UArduinoSerialPortUtil

| API | Описание |
|-----|----------|
| `normalizeDevicePath` | `ttyACM0` → `/dev/ttyACM0` |
| `preferredOpenName` | Лучшее имя для `QSerialPort::open` |
| `listPortsSorted` | USB-порты сверху, host UART серым в GUI |

## UArduinoFlasher

**Файлы:** `UArduinoFlasher.{h,cpp}`

| API | Описание |
|-----|----------|
| `locateAvrdudeBinary` | PATH, `AVRDUDE`, `~/.arduino15/.../avrdude` |
| `locateAvrdudeConf` | `AVRUDUDE_CONF`, `/etc`, рядом с бинарником |
| `buildCommand` | Строка аргументов avrdude |
| `flash(profile, port, hex, &err)` | `QProcess`, парсинг `%` в stdout → `progressChanged` |

**Сигналы:** `progressChanged(int)`, `finished(bool, message)`.

При ошибке permission — расширенное сообщение в `errorOut`.

## UArduinoBoardProfile

**Файлы:** `UArduinoBoardProfile.{h,cpp}`

| Kind | MCU | Protocol | Upload baud |
|------|-----|----------|-------------|
| Uno | atmega328p | stk500 | 115200 |
| Mega2560 | atmega2560 | stk500v2 | 115200 |

Используется в `UArduinoBoard::RunUpload()`.

## UFirmwareManifest

**Файлы:** `UFirmwareManifest.{h,cpp}`

- `firmwareRoot()` — `RDK_HARDWARE_FIRMWARE_DIR`, рядом с приложением, `NMSDK_SOURCE_DIR`
- `resolveBundledHex(bundled_id, board_profile_int)` / `resolveBundledHex(bundled_id, board_key)` — путь к `.hex` из `manifest.json`

## См. также

- [firmware_build.md](firmware_build.md)
- [Architecture.md](Architecture.md)
