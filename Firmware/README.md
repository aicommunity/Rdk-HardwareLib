# Bundled firmware (sources + dev copy)

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
- [ ] Установлен `avrdude` (Linux: пакет `avrdude`; Windows: идёт с Arduino / toolchain).
- [ ] В модели выбран верный **Board profile**: `0` = Uno, `1` = Mega 2560.
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
