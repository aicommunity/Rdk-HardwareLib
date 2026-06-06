# Bundled firmware (sources + dev copy)

## RU

**Runtime (по умолчанию в приложении):** `Bin/ArduinoFirmware/` — см. `Bin/ArduinoFirmware/README.md`.

| Path | Description |
|------|-------------|
| `sensor_lab/sensor_lab.ino` | Custom protocol sketch (57600 baud) |
| `sensor_lab/uno.hex`, `mega2560.hex` | Prebuilt sensor_lab (копируются в `Bin/ArduinoFirmware` скриптом сборки) |
| `firmata/standard_firmata_*.hex` | StandardFirmata for Uno / Mega |
| `manifest.json` | Шаблон manifest (копируется в `Bin/ArduinoFirmware`) |

Build all HEX files:

```bash
./Scripts/build_arduino_firmware.sh
```

If `downloads.arduino.cc` returns 403, build on a machine with working Arduino CLI and commit the `.hex` files.

## Чеклист ручной проверки на железе

Используйте NeuroModeler (GUI) или конфиг с компонентами `ArduinoBoard` + `ArduinoSensorSketch` / `ArduinoFirmata`.  
Скорость порта по умолчанию: **57600**. Перед прошивкой закройте Serial Monitor в Arduino IDE.

### Подготовка

- [ ] Плата подключена по USB, в системе виден порт (`/dev/ttyACM0`, `COM3`, …).
- [ ] **Windows:** запущен `Bin/Platform/Win/SetupArduinoTools.bat` или `avrdude` в PATH.
- [ ] **Linux:** установлен пакет `avrdude`; пользователь в группе `dialout` (`sudo usermod -aG dialout $USER`).
- [ ] В модели выбран верный **Board profile**: `0` = Uno, `1` = Mega 2560 (GUI: авто-детект при выборе COM для официальных Arduino; CH340-клоны — вручную).
- [ ] Bundled HEX существуют (см. таблицу выше) или указан свой путь в **Firmware path**.

### A. Прошивка sensor_lab (`sensor_lab_v1`)

Компонент: **ArduinoBoard**.

1. [ ] `BundledFirmwareId` = `sensor_lab_v1` (или пустой `FirmwarePath` — возьмётся из manifest).
2. [ ] `PortName` = ваш COM/tty.
3. [ ] Нажать **Upload firmware** (или `UploadFirmwareFlag` + Calculate).
4. [ ] `UploadProgress` → 100, `UploadLastResult` без ошибки.
5. [ ] После upload плата перезагрузилась; повторно открыть порт при необходимости.

**Ожидание:** avrdude завершается с `avrdude done.  Thank you.` (или аналог в логе).

### B. Кастомный протокол (ArduinoSensorSketch)

Компонент: **ArduinoSensorSketch** (может наследовать порт от Board или задать свой).

1. [ ] `ConnectOnBuild` = true, `BaudRate` = 57600, **Connect** / Calculate.
2. [ ] `ConnectionState` = 2 (Connected), `LastError` пустой.
3. [ ] Отправить preset **GET STATUS** — в логе/отладке нет ошибки парсера.
4. [ ] Включить **Get data from buffers**, Calculate — в GUI таблица **Readings** заполняется (или `DoubleMatrixReadings` не пустая).
5. [ ] **Get pins info** — `PinStatusJson` / ответ 0x04; на диаграмме подсвечены D2, A2, D9.
6. [ ] Команды **START READING** / **STOP READING**, **ROTATE** / **STOP ROTATE** — плата реагирует (серво/чтение), без зависания порта.

**Опционально (PROTO 2):** `ProtocolVersion` = 2, переподключение — хост шлёт `PROTO 2`, прошивка отвечает `PROTO OK 2`, кадры принимаются (счётчик `RxFrameCount` растёт).

### C. Прошивка StandardFirmata (`standard_firmata`)

1. [ ] На **ArduinoBoard**: `BundledFirmwareId` = `standard_firmata`, upload как в блоке A.
2. [ ] Компонент **ArduinoFirmata**, тот же порт и 57600.
3. [ ] После Calculate: `FirmataReady` = true, `FirmataFirmwareVersion` не пустой (например `2.x`).
4. [ ] Клик по пину на диаграмме → `SelectedPin` обновился.
5. [ ] **Set pin mode** (Output) + **Write digital** на светодиодном пине (Uno: D13) — LED переключается.
6. [ ] **Read analog** на A0 (потенциометр/земля) — `AnalogPinValue` меняется.

### D. Heartbeat и переподключение (ArduinoBoard)

1. [ ] `HeartbeatEnabled` = true, интервал/таймаут по умолчанию (3 s / 10 s).
2. [ ] **Health check** — `MissedHeartbeats` не растёт при живой связи.
3. [ ] Отключить USB на 15 s, включить снова: при `AutoReconnect` = true порт восстанавливается (или вручную Refresh + Connect).

### E. Mega 2560 (если есть плата)

- [ ] Upload `mega2560.hex` / Firmata mega — без ошибки avrdude (`stk500v2`).
- [ ] Sensor sketch и Firmata работают так же, как на Uno.

### F. Регрессия GUI

- [ ] Открывается контроллер **ArduinoBoard** — диаграмма Uno/Mega, чекбоксы Connect/Heartbeat.
- [ ] **ArduinoSensorSketch** — матрица и роли пинов DHT/Hall/Servo.
- [ ] **ArduinoFirmata** — клик по пину на диаграмме.

### Типичные проблемы

| Симптом | Что проверить |
|--------|----------------|
| Upload failed | Порт не занят IDE Monitor; права на `/dev/ttyACM*`; верный Board profile |
| Connected, но нет данных | 57600; та же прошивка (sensor_lab vs Firmata); DTR/перезагрузка после flash |
| Firmata not ready | Прошит `standard_firmata`, не sensor_lab |
| Пустая матрица | `GetDataFromBuffers` + `START READING`; датчик DHT на D2 |

После прохождения чеклиста отметьте дату, плату и ОС в комментарии к issue/PR.

## Готовые конфигурации NeuroModeler

Минимальные проекты для каждого ClassName: `Bin/Configs/SpikeSamples/Hardware/` (см. [README.md](../../../Bin/Configs/SpikeSamples/Hardware/README.md)).

Перегенерация: `python3 Scripts/generate_arduino_hardware_configs.py`

---

## EN

**Runtime (default in the application):** `Bin/ArduinoFirmware/` — see `Bin/ArduinoFirmware/README.md`.

| Path | Description |
|------|-------------|
| `sensor_lab/sensor_lab.ino` | Custom protocol sketch (57600 baud) |
| `sensor_lab/uno.hex`, `mega2560.hex` | Prebuilt sensor_lab (copied to `Bin/ArduinoFirmware` by the build script) |
| `firmata/standard_firmata_*.hex` | StandardFirmata for Uno / Mega |
| `manifest.json` | Manifest template (copied to `Bin/ArduinoFirmware`) |

Build all HEX files:

```bash
./Scripts/build_arduino_firmware.sh
```

If `downloads.arduino.cc` returns 403, build on a machine with working Arduino CLI and commit the `.hex` files.

## Hardware manual verification checklist

Use NeuroModeler (GUI) or a config with `ArduinoBoard` + `ArduinoSensorSketch` / `ArduinoFirmata` components.  
Default port speed: **57600**. Close Serial Monitor in Arduino IDE before flashing.

### Preparation

- [ ] Board connected via USB, port visible in the system (`/dev/ttyACM0`, `COM3`, …).
- [ ] **Windows:** `Bin/Platform/Win/SetupArduinoTools.bat` run or `avrdude` in PATH.
- [ ] **Linux:** `avrdude` package installed; user in `dialout` group (`sudo usermod -aG dialout $USER`).
- [ ] Correct **Board profile** selected in the model: `0` = Uno, `1` = Mega 2560 (GUI: auto-detect when selecting COM for official Arduino; CH340 clones — manual).
- [ ] Bundled HEX exist (see table above) or a custom path is set in **Firmware path**.

### A. Flash sensor_lab (`sensor_lab_v1`)

Component: **ArduinoBoard**.

1. [ ] `BundledFirmwareId` = `sensor_lab_v1` (or empty `FirmwarePath` — taken from manifest).
2. [ ] `PortName` = your COM/tty.
3. [ ] Press **Upload firmware** (or `UploadFirmwareFlag` + Calculate).
4. [ ] `UploadProgress` → 100, `UploadLastResult` without error.
5. [ ] After upload the board rebooted; reopen the port if needed.

**Expected:** avrdude finishes with `avrdude done.  Thank you.` (or equivalent in the log).

### B. Custom protocol (ArduinoSensorSketch)

Component: **ArduinoSensorSketch** (may inherit port from Board or set its own).

1. [ ] `ConnectOnBuild` = true, `BaudRate` = 57600, **Connect** / Calculate.
2. [ ] `ConnectionState` = 2 (Connected), `LastError` empty.
3. [ ] Send preset **GET STATUS** — no parser error in log/debug.
4. [ ] Enable **Get data from buffers**, Calculate — GUI **Readings** table fills (or `DoubleMatrixReadings` is non-empty).
5. [ ] **Get pins info** — `PinStatusJson` / response 0x04; D2, A2, D9 highlighted on the diagram.
6. [ ] **START READING** / **STOP READING**, **ROTATE** / **STOP ROTATE** commands — board responds (servo/reading), port does not hang.

**Optional (PROTO 2):** `ProtocolVersion` = 2, reconnect — host sends `PROTO 2`, firmware replies `PROTO OK 2`, frames accepted (`RxFrameCount` increases).

### C. Flash StandardFirmata (`standard_firmata`)

1. [ ] On **ArduinoBoard**: `BundledFirmwareId` = `standard_firmata`, upload as in block A.
2. [ ] **ArduinoFirmata** component, same port and 57600.
3. [ ] After Calculate: `FirmataReady` = true, `FirmataFirmwareVersion` non-empty (e.g. `2.x`).
4. [ ] Click a pin on the diagram → `SelectedPin` updated.
5. [ ] **Set pin mode** (Output) + **Write digital** on LED pin (Uno: D13) — LED toggles.
6. [ ] **Read analog** on A0 (potentiometer/ground) — `AnalogPinValue` changes.

### D. Heartbeat and reconnect (ArduinoBoard)

1. [ ] `HeartbeatEnabled` = true, default interval/timeout (3 s / 10 s).
2. [ ] **Health check** — `MissedHeartbeats` does not increase with live connection.
3. [ ] Disconnect USB for 15 s, reconnect: with `AutoReconnect` = true port restores (or manually Refresh + Connect).

### E. Mega 2560 (if board available)

- [ ] Upload `mega2560.hex` / Firmata mega — no avrdude error (`stk500v2`).
- [ ] Sensor sketch and Firmata work the same as on Uno.

### F. GUI regression

- [ ] **ArduinoBoard** controller opens — Uno/Mega diagram, Connect/Heartbeat checkboxes.
- [ ] **ArduinoSensorSketch** — matrix and DHT/Hall/Servo pin roles.
- [ ] **ArduinoFirmata** — click pin on diagram.

### Common issues

| Symptom | What to check |
|---------|---------------|
| Upload failed | Port not busy with IDE Monitor; permissions on `/dev/ttyACM*`; correct Board profile |
| Connected but no data | 57600; same firmware (sensor_lab vs Firmata); DTR/reboot after flash |
| Firmata not ready | Flashed `standard_firmata`, not sensor_lab |
| Empty matrix | `GetDataFromBuffers` + `START READING`; DHT sensor on D2 |

After completing the checklist, note date, board, and OS in an issue/PR comment.

## Ready-made NeuroModeler configurations

Minimal projects for each ClassName: `Bin/Configs/SpikeSamples/Hardware/` (see [README.md](../../../Bin/Configs/SpikeSamples/Hardware/README.md)).

Regeneration: `python3 Scripts/generate_arduino_hardware_configs.py`
