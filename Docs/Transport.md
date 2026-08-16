# Transport layer

## RU

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
| `locateAvrdudeBinary` | `AVRDUDE` → PATH → **`ArduinoTools/bin` рядом с exe** → `~/.arduino15/.../avrdude` |
| `locateAvrdudeConf` | `AVRUDUDE_CONF` → **`ArduinoTools/etc/avrdude.conf`** → `/etc/avrdude.conf` → рядом с бинарником → `.arduino15` |
| `buildCommand` | Строка аргументов avrdude |
| `flash(profile, port, hex, &err)` | `QProcess`, парсинг `%` в stdout → `progressChanged` (блокирующий вызов) |

**Сигналы:** `progressChanged(int)`, `finished(bool, message)`.

**Async upload:** `UArduinoUploadJob::runSync` на `QThread`; `UArduinoBoard::PollUploadJob` в `ACalculate`. Env `ARDUINO_SYNC_UPLOAD=1` — синхронный `RunUploadBlocking` (интеграционные тесты).

При ошибке permission или `not in sync` / `not responding` — расширенное сообщение в `errorOut`.

Перед upload: `CloseConnection`, пауза ~400 ms, сброс в bootloader (1200 baud + DTR на Uno/клонах), avrdude на Windows использует `-P "\\.\COMn"`.

### Windows setup

Запустите [`Scripts/setup_arduino_tools.ps1`](../../../Scripts/setup_arduino_tools.ps1) (wrapper [`Bin/Platform/Win/SetupArduinoTools.bat`](../../../Bin/Platform/Win/SetupArduinoTools.bat)) — копирует `avrdude` в `Bin/Platform/Win/ArduinoTools/`. Подробнее: [Arduino-Setup-Windows.md](Arduino-Setup-Windows.md).

### Linux

Установите пакетный `avrdude` (`apt` / `dnf` / `pacman`), добавьте пользователя в `dialout`. Каталог `Bin/Platform/Linux/ArduinoTools` **не требуется**.

## UArduinoBoardProfile

**Файлы:** `UArduinoBoardProfile.{h,cpp}`

| Kind | MCU | Protocol | Upload baud |
|------|-----|----------|-------------|
| Uno | atmega328p | arduino | 115200 |
| Mega2560 | atmega2560 | wiring | 115200 |

Mega2560 `wiring` matches Arduino IDE / avrdude 8.x bundled tools (`-cwiring`).

Используется в `UArduinoBoard::RunUpload()` / async upload job.

## UFirmwareManifest

**Файлы:** `UFirmwareManifest.{h,cpp}`

- `bundledFirmwareRelativeRoot()` — `../../ArduinoFirmware` (`Bin/ArduinoFirmware`) относительно `Bin/Platform/<OS>/`
- `bundledHexRelativePath(bundled_id, board_profile)` — относительный путь к `.hex` для значения по умолчанию в `FirmwarePath`
- `resolveFromApplicationDir(relative)` — разворачивает путь от каталога exe
- `firmwareRoot()` — `RDK_HARDWARE_FIRMWARE_DIR`, затем относительный корень, затем `NMSDK_SOURCE_DIR`
- `resolveBundledHex(...)` — абсолютный путь к `.hex` из `manifest.json`

## См. также

- [firmware_build.md](firmware_build.md)
- [Architecture.md](Architecture.md)

---

## EN

Internal classes in `Core/Transport/` (not registered in Storage).

## UArduinoSerialSession

**Files:** `UArduinoSerialSession.{h,cpp}`

| API | Description |
|-----|-------------|
| `open(port, baud)` | Multiple candidate names (systemLocation, normalized path) |
| `close()` | |
| `isOpen()` | |
| `bytesToWrite()` | Command queue waits for `== 0` before next write |
| `write(data)` | |
| `takeReceivedBytes()` | Swap RX buffer (thread-safe) |
| `lastError()` | Text on failed open |
| `availablePorts()` | Path list via `UArduinoSerialPortUtil` |

**Signals:** `bytesReceived`, `errorOccurred`.

**Threading:** `readyRead` only writes to a mutex-protected buffer. In HardwareLib there is **no** `connect(bytesReceived, …)` to `UNet` — components fetch data in `UArduinoBoard::ACalculate` via `takeReceivedBytes()` (engine thread). See [Architecture.md](Architecture.md).

**Linux permissions:** on `Permission denied` in `lastError` — hint about the `dialout` group.

## UArduinoSerialPortUtil

| API | Description |
|-----|-------------|
| `normalizeDevicePath` | `ttyACM0` → `/dev/ttyACM0` |
| `preferredOpenName` | Best name for `QSerialPort::open` |
| `listPortsSorted` | USB ports on top, host UART grayed in GUI |

## UArduinoFlasher

**Files:** `UArduinoFlasher.{h,cpp}`

| API | Description |
|-----|-------------|
| `locateAvrdudeBinary` | `AVRDUDE` → PATH → **`ArduinoTools/bin` next to exe** → `~/.arduino15/.../avrdude` |
| `locateAvrdudeConf` | `AVRUDUDE_CONF` → **`ArduinoTools/etc/avrdude.conf`** → `/etc/avrdude.conf` → next to binary → `.arduino15` |
| `buildCommand` | avrdude argument string |
| `flash(profile, port, hex, &err)` | `QProcess`, parse `%` in stdout → `progressChanged` (blocking call) |

**Signals:** `progressChanged(int)`, `finished(bool, message)`.

**Async upload:** `UArduinoUploadJob::runSync` on `QThread`; `UArduinoBoard::PollUploadJob` in `ACalculate`. Env `ARDUINO_SYNC_UPLOAD=1` — synchronous `RunUploadBlocking` (integration tests).

On permission error or `not in sync` / `not responding` — extended message in `errorOut`.

Before upload: `CloseConnection`, pause ~400 ms, bootloader reset (1200 baud + DTR on Uno/clones), avrdude on Windows uses `-P "\\.\COMn"`.

### Windows setup

Run [`Scripts/setup_arduino_tools.ps1`](../../../Scripts/setup_arduino_tools.ps1) (wrapper [`Bin/Platform/Win/SetupArduinoTools.bat`](../../../Bin/Platform/Win/SetupArduinoTools.bat)) — copies `avrdude` to `Bin/Platform/Win/ArduinoTools/`. Details: [Arduino-Setup-Windows.md](Arduino-Setup-Windows.md).

### Linux

Install packaged `avrdude` (`apt` / `dnf` / `pacman`), add user to `dialout`. Directory `Bin/Platform/Linux/ArduinoTools` is **not required**.

## UArduinoBoardProfile

**Files:** `UArduinoBoardProfile.{h,cpp}`

| Kind | MCU | Protocol | Upload baud |
|------|-----|----------|-------------|
| Uno | atmega328p | arduino | 115200 |
| Mega2560 | atmega2560 | wiring | 115200 |

Mega2560 `wiring` matches Arduino IDE / avrdude 8.x bundled tools (`-cwiring`).

Used in `UArduinoBoard::RunUpload()` / async upload job.

## UFirmwareManifest

**Files:** `UFirmwareManifest.{h,cpp}`

- `bundledFirmwareRelativeRoot()` — `../../ArduinoFirmware` (`Bin/ArduinoFirmware`) relative to `Bin/Platform/<OS>/`
- `bundledHexRelativePath(bundled_id, board_profile)` — relative path to `.hex` for default `FirmwarePath`
- `resolveFromApplicationDir(relative)` — resolves path from exe directory
- `firmwareRoot()` — `RDK_HARDWARE_FIRMWARE_DIR`, then relative root, then `NMSDK_SOURCE_DIR`
- `resolveBundledHex(...)` — absolute path to `.hex` from `manifest.json`

## See also

- [firmware_build.md](firmware_build.md)
- [Architecture.md](Architecture.md)
