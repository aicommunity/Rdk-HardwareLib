# Статус документации Rdk-HardwareLib

## RU

Краткий статус документации библиотеки Rdk-HardwareLib и чеклист синхронизации с кодом.

**Аудит:** 2026-06-06 — [Component-Gap-Report.md](../../../Docs/Audit/Component-Gap-Report.md), [Code-Inventory.json](../../../Docs/Audit/Code-Inventory.json)  
**Обновление Track 3:** 2026-07-27

**Сверка 2026-10-08:** локальный checkout содержит 11 регистраций `UploadClass`; этот статус синхронизирован с `Core/UHardwareLibrary.cpp`.

---

## Статистика (сверка с кодом)

| Метрика | Значение |
|---------|----------|
| Зарегистрированных классов (`UploadClass`) | **11** |
| Файлов `Docs/Components/*.md` | **15** |
| Классов без doc-файла | **0** |
| Orphan docs (legacy/групповые) | **4** |
| Отсутствуют в Component-Catalog | **0** |
| Регистрация | `Core/UHardwareLibrary.cpp` |

**Прогресс по классам:** все `UploadClass` имеют per-class документацию.\
**Bilingual parity (Track 3):** ✅ `## EN` добавлен в Architecture, API-Overview, Usage-Examples, Component-Catalog, Transport, Protocol, GUI, firmata_spike и в пять per-class компонентов на дату Track 3; DeviceIO/CustomFirmware/Esp32/Wheeled/WaveRover — отдельные RU/EN.

---

## Выполнено

### Общая документация
- ✅ **README.md** — обзор, быстрый старт, соглашения именования (RU/EN)
- ✅ **Architecture.md** — архитектура transport/protocol/GUI (RU/EN)
- ✅ **API-Overview.md** — edge-свойства, публичный API (RU/EN)
- ✅ **Usage-Examples.md** — примеры конфигураций (RU/EN)
- ✅ **Component-Catalog.md** — полный каталог с таблицами (RU/EN)
- ✅ **Transport.md**, **Protocol.md** — serial-сессия, Firmata, binary parser (RU/EN)
- ✅ **GUI.md** — Qt-контроллеры и pinout (RU/EN)
- ✅ **firmware_build.md**, **firmata_spike.md**, **Arduino-Setup-Windows.md**
- ✅ **TechDebt.md**, **FirmataTechDebt.md**, **DOXYGEN.md**
- ✅ **Diagrams/README.md**
- ✅ **Legacy/** — редиректы со старых ClassName

### Компоненты (все зарегистрированные классы, RU/EN)
- ✅ **ArduinoAdc** — [`Components/ArduinoAdc.md`](Components/ArduinoAdc.md)
- ✅ **ArduinoBoard** — [`Components/ArduinoBoard.md`](Components/ArduinoBoard.md)
- ✅ **Esp32Board** — [`Components/Esp32Board.md`](Components/Esp32Board.md)
- ✅ **ArduinoWheeledRobot** / **Esp32WheeledRobot** / **WaveRover** — Components + [WheeledRobots.md](WheeledRobots.md) + [Protocol-WaveshareUgvJson.md](Protocol-WaveshareUgvJson.md)
- ✅ **ArduinoDcDemo** — [`Components/ArduinoDcDemo.md`](Components/ArduinoDcDemo.md)
- ✅ **ArduinoFirmata** — [`Components/ArduinoFirmata.md`](Components/ArduinoFirmata.md)
- ✅ **ArduinoSensorSketch** — [`Components/ArduinoSensorSketch.md`](Components/ArduinoSensorSketch.md)

- ✅ **ArduinoDeviceIO** — [Components/ArduinoDeviceIO.md](Components/ArduinoDeviceIO.md)
- ✅ **ArduinoCustomFirmware** — [Components/ArduinoCustomFirmware.md](Components/ArduinoCustomFirmware.md)

### Orphan docs (ожидаемые legacy)
- `ADC` → миграция на `ArduinoAdc`
- `Arduino` → миграция на `ArduinoBoard` + `ArduinoSensorSketch` / `ArduinoFirmata`
- `ArduinoConnect` → legacy transport
- `DC` → миграция на `ArduinoDcDemo`

См. таблицу миграции в [Component-Catalog.md](Component-Catalog.md) и [Legacy/README.md](Legacy/README.md).

### Firmware и тесты
- ✅ Манифест `Firmware/manifest.json`: StandardFirmata и восемь AVR sketches (Uno/Mega)
- ✅ Тестовые конфиги: `Bin/Configs/SpikeSamples/Hardware/`
- ✅ Интеграционные тесты: `Test_ArduinoBoardEdges`, `Test_ArduinoFirmataClient`, `Test_ArduinoHardwareIntegration` и др.
- ✅ Windows MSVC 2019: выбранный набор HardwareLib unit-тестов — 15/15; AVR firmware matrix Uno/Mega — собрана
- ⚠️ ESP32 firmware не проверена сборкой; физические стендовые проверки не выполнялись

---

## Планируется / рекомендации

### Приоритет P2 — качество компонентной документации
- [ ] Создать **Component-Documentation-Template.md** (UML, edge-свойства, примеры XML)
- [ ] В каждом компоненте — таблицы всех `UProperty` из заголовков `Core/*.h`
- [ ] В blockе «Использование» — ссылки на `Bin/Configs/SpikeSamples/Hardware/`

### Приоритет P3 — навигация
- [ ] В начале каждого компонента — «Каталог: [Component-Catalog.md](Component-Catalog.md)»
- [ ] Пометить legacy-файлы (`ADC.md`, `Arduino.md`, …) явным баннером «deprecated → new ClassName»

---

## Чеклист по классам (`UploadClass`)

Регистрация: `Libraries/Rdk-HardwareLib/Core/UHardwareLibrary.cpp`.

| ClassName | Doc-файл | В каталоге | EN | Статус |
|-----------|----------|------------|----|--------|
| `ArduinoAdc` | `Components/ArduinoAdc.md` | ✅ | ✅ | ✅ |
| `ArduinoBoard` | `Components/ArduinoBoard.md` | ✅ | ✅ | ✅ |
| `ArduinoCustomFirmware` | `Components/ArduinoCustomFirmware.md` | ✅ | ✅ | ✅ |
| `ArduinoDeviceIO` | `Components/ArduinoDeviceIO.md` | ✅ | ✅ | ✅ |
| `ArduinoDcDemo` | `Components/ArduinoDcDemo.md` | ✅ | ✅ | ✅ |
| `ArduinoFirmata` | `Components/ArduinoFirmata.md` | ✅ | ✅ | ✅ |
| `ArduinoSensorSketch` | `Components/ArduinoSensorSketch.md` | ✅ | ✅ | ✅ |
| `ArduinoWheeledRobot` | `Components/ArduinoWheeledRobot.md` | ✅ | ✅ | ✅ |
| `Esp32Board` | `Components/Esp32Board.md` | ✅ | ✅ | ✅ |
| `Esp32WheeledRobot` | `Components/Esp32WheeledRobot.md` | ✅ | ✅ | ✅ |
| `WaveRover` | `Components/WaveRover.md` | ✅ | ✅ | ✅ |

**Прогресс: 11/11 (100%)** ✅

---

## Чеклист синхронизации с кодом

При добавлении или изменении класса в `Libraries/Rdk-HardwareLib/Core/` рекомендуется обновить:

1. **Component-Catalog.md** — при регистрации в `UHardwareLibrary.cpp`
2. **Docs/Components/[ComponentName].md** — свойства, edge-семантика, примеры XML
3. **API-Overview.md** — при изменении property-driven control или публичного API
4. **Transport.md** / **Protocol.md** — при изменении serial/Firmata/binary протокола
5. **GUI.md** — при изменении Qt-контроллеров или pinout-ресурсов
6. **firmware_build.md** / `Firmware/manifest.json` — при добавлении прошивок
7. **Usage-Examples.md** и `Bin/Configs/SpikeSamples/Hardware/` — при новых сценариях
8. **Legacy/** — при переименовании `ClassName`; обновить `Scripts/migrate_arduino_classnames.py`
9. Перезапустить аудит: `Scripts/doc-audit/run-all.sh`

---

## EN

### Documentation status summary

Rdk-HardwareLib has **11** registered `UploadClass` entries; **all 11** have per-class component documentation with RU/EN. Infrastructure docs (Architecture, API-Overview, Usage-Examples, Component-Catalog, Transport, Protocol, GUI, firmata_spike) include `## EN`. **4** orphan docs are expected legacy redirects.

**Progress:** 11/11 classes (100%) ✅ · bilingual Track 3 infra/components: done

Windows verification on 2026-10-08: MSVC 2019 selected HardwareLib tests 15/15 passed; AVR firmware matrix for Uno/Mega compiled. ESP32 firmware and physical hardware remain unverified.

### Remaining work

- Introduce Component-Documentation-Template.md for consistent UML and property tables
- Keep firmware manifest and SpikeSamples Hardware configs in sync with docs

### Sync checklist (code changes)

Update Component-Catalog, component docs, API-Overview, Transport/Protocol/GUI docs, firmware manifest, and SpikeSamples configs when changing `Core/` or firmware; re-run doc audit.

---

*References: [Component-Gap-Report.md](../../../Docs/Audit/Component-Gap-Report.md), [Code-Inventory.json](../../../Docs/Audit/Code-Inventory.json); template — [Nmsdk-MotionControlLib/Docs/Documentation-Status.md](../../Nmsdk-MotionControlLib/Docs/Documentation-Status.md).*
