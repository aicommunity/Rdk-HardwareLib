# GUI — Rdk-HardwareLib.gui

## RU

Статическая библиотека **Rdk-HardwareLib.gui** (CMake target), линкуется из NeuroModeler и NeuroModelerConsole.

## Регистрация форм

[`HardwareLibComponentGuiRegistration.cpp`](../GUI/Qt/HardwareLibComponentGuiRegistration.cpp) вызывает `RegisterHardwareLibComponentGuiForms()`:

| ClassName | formId | Widget |
|-----------|--------|--------|
| `ArduinoBoard` | `hw.arduino.board` | `HardwareArduinoBoardControllerWidget` |
| `ArduinoSensorSketch` | `hw.arduino.sensor_sketch` | `HardwareArduinoSensorSketchControllerWidget` |
| `ArduinoFirmata` | `hw.arduino.firmata` | `HardwareArduinoFirmataControllerWidget` |
| `ArduinoDcDemo` | `hw.arduino.dc_demo` | `HardwareArduinoDcDemoControllerWidget` |
| `ArduinoAdc` | `hw.arduino.adc` | `HardwareArduinoAdcControllerWidget` |

## Доступ к модели

[`HardwareGuiHelpers`](../GUI/Qt/widgets/HardwareGuiHelpers.h):

- `getProp` / `setProp` — чтение/запись свойств в **UTF-8** при `RDK_UNICODE_RUN` (см. `UArduinoPropertyString`); в строках UI для MSVC — escape `\u2014` / `\u2026`, не литералы «—»/«…» в исходнике
- `applyUnicodeFriendlyFont` — шрифт приложения для корректного отображения кириллицы в комбобоксах и логах
- `envCalculate` — один тик расчёта
- **`pulseEdge(ctx, "Connect")`** — `setProp(edge, "1")` + `envCalculate` (edge сбрасыется в C++)

Кнопки Connect/Disconnect/Reconnect/Upload **не** вызывают C++ напрямую — только property API.

## Вкладка Board

[`HardwareArduinoBoardPanelWidget`](../GUI/Qt/widgets/HardwareArduinoBoardPanelWidget.cpp) — общая панель:

- Порт, baud, bundled firmware, upload (асинхронный: `QTimer` 200 ms + `envCalculate` для `PollUploadJob`)
- **Board profile** Uno / Mega; **Auto-detect board when port changes** (USB VID/PID + description)
- **Upload preview** — MCU, avrdude `-c`, HEX; предупреждение при несоответствии профиля и HEX
- Progress bar: indeterminate на фазе bootloader, затем `%`; кнопки disabled при `IsUploading`
- Connect / Disconnect / Reconnect / Health (`pulseEdge`)
- Статус: `IsConnected`, `HasError`, `LastError`, `UploadLastResult` (read-only); при **выключенном** auto-detect — строка `Detected: Arduino Uno` / `Mega 2560` / `unknown` по выбранному COM (USB VID/PID + description)
- **Disconnect не очищает `PortName`**

Встроена во вкладку **Board** у SensorSketch, Firmata, DcDemo.

## Assembly

Вкладка **Assembly** (`HardwareArduinoAssemblyTabHost`): редактор Hardware Setup + `UArduinoAssemblyViewWidget` (схема сборки, Export SVG). Подробнее: [Assembly-View.md](Assembly-View.md).

- Board: вкладки **Assembly** | **Pinout** + панель Board
- SensorSketch / Firmata / DcDemo: вкладка **Assembly** рядом с Board
- Adc: Assembly **read-only** (setup с компонента; без записи props)
- Editor: смена board/firmware, **Add/Remove device**, правка Port/Module/Channel/Role → Validate → rebuild view

## Diagram

[`UArduinoBoardDiagramWidget`](../GUI/Qt/widgets/UArduinoBoardDiagramWidget.cpp):

- SVG pinout Uno / Mega (`hardware_lib.qrc`)
- Overlay пинов из `uno_pins.json`, `mega2560_pins.json`
- Firmata: клик по пину → `SelectedPin`

## Sensor sketch widget

Вкладки **Sensor** | **Board**. Sensor: команды (`pulseEdge("SendCommand")`), presets, matrix.

## Firmata widget

Вкладки **Pins** | **Monitor** | **I2C** | **Board**. Пульт пинов: `SetPinMode`, `WriteDigital`, `ReadAnalog`, пресеты, `Monitor all` → `AutoRefreshPins`. Monitor: превью `AnalogSamples`, `StreamLog`. Диаграмма: `applyPinStatusJson` + клик по пину (Mega — полный `mega2560_pins.json`).

**Watch:** привяжите downstream к свойству `AnalogSamples` (`ptOutput`) для графика/статистики через `UWatch` (без отдельного legacy Graph widget).

## DcDemo widget

Вкладки **DC** | **Board**. Send → `SendCommand`, Read speed → `GetSpeed`.

## Adc widget

`LinkedFirmataName`, выбор пина по label (`UArduinoPinMap`), `UseLinkedAnalogSamples` (читать `AnalogSamples` связанного Firmata) или `ReadAdcFlag` для one-shot.

## Ресурсы

- `GUI/Qt/Resources/hardware_lib.qrc`
- Fallback с диска при `NMSDK_ROOT` для dev без пересборки qrc

## Сборка

Требуется Qt **Svg**. Без Svg — warning в CMake, diagram недоступен.

## См. также

- [Usage-Examples.md](Usage-Examples.md)
- [Architecture.md](Architecture.md) — threading

---

## EN

Static library **Rdk-HardwareLib.gui** (CMake target), linked from NeuroModeler and NeuroModelerConsole.

## Form registration

[`HardwareLibComponentGuiRegistration.cpp`](../GUI/Qt/HardwareLibComponentGuiRegistration.cpp) calls `RegisterHardwareLibComponentGuiForms()`:

| ClassName | formId | Widget |
|-----------|--------|--------|
| `ArduinoBoard` | `hw.arduino.board` | `HardwareArduinoBoardControllerWidget` |
| `ArduinoSensorSketch` | `hw.arduino.sensor_sketch` | `HardwareArduinoSensorSketchControllerWidget` |
| `ArduinoFirmata` | `hw.arduino.firmata` | `HardwareArduinoFirmataControllerWidget` |
| `ArduinoDcDemo` | `hw.arduino.dc_demo` | `HardwareArduinoDcDemoControllerWidget` |
| `ArduinoAdc` | `hw.arduino.adc` | `HardwareArduinoAdcControllerWidget` |

## Model access

[`HardwareGuiHelpers`](../GUI/Qt/widgets/HardwareGuiHelpers.h):

- `getProp` / `setProp` — read/write properties in **UTF-8** under `RDK_UNICODE_RUN` (see `UArduinoPropertyString`); in UI strings for MSVC — escape `\u2014` / `\u2026`, not literal «—»/«…» in source
- `applyUnicodeFriendlyFont` — application font for correct Cyrillic in comboboxes and logs
- `envCalculate` — one calculation tick
- **`pulseEdge(ctx, "Connect")`** — `setProp(edge, "1")` + `envCalculate` (edge resets in C++)

Connect/Disconnect/Reconnect/Upload buttons **do not** call C++ directly — only property API.

## Board tab

[`HardwareArduinoBoardPanelWidget`](../GUI/Qt/widgets/HardwareArduinoBoardPanelWidget.cpp) — shared panel:

- Port, baud, bundled firmware, upload (async: `QTimer` 200 ms + `envCalculate` for `PollUploadJob`)
- **Board profile** Uno / Mega; **Auto-detect board when port changes** (USB VID/PID + description)
- **Upload preview** — MCU, avrdude `-c`, HEX; warning on profile/HEX mismatch
- Progress bar: indeterminate during bootloader phase, then `%`; buttons disabled when `IsUploading`
- Connect / Disconnect / Reconnect / Health (`pulseEdge`)
- Status: `IsConnected`, `HasError`, `LastError`, `UploadLastResult` (read-only); with auto-detect **off** — line `Detected: Arduino Uno` / `Mega 2560` / `unknown` for selected COM (USB VID/PID + description)
- **Disconnect does not clear `PortName`**

Embedded in **Board** tab of SensorSketch, Firmata, DcDemo.

## Diagram

[`UArduinoBoardDiagramWidget`](../GUI/Qt/widgets/UArduinoBoardDiagramWidget.cpp):

- SVG pinout Uno / Mega (`hardware_lib.qrc`)
- Pin overlay from `uno_pins.json`, `mega2560_pins.json`
- Firmata: pin click → `SelectedPin`

## Sensor sketch widget

Tabs **Sensor** | **Board**. Sensor: commands (`pulseEdge("SendCommand")`), presets, matrix.

## Firmata widget

Tabs **Pins** | **Monitor** | **I2C** | **Board**. Pin console: `SetPinMode`, `WriteDigital`, `ReadAnalog`, presets, `Monitor all` → `AutoRefreshPins`. Monitor: preview `AnalogSamples`, `StreamLog`. Diagram: `applyPinStatusJson` + pin click (Mega — full `mega2560_pins.json`).

**Watch:** bind downstream to `AnalogSamples` property (`ptOutput`) for chart/statistics via `UWatch` (no separate legacy Graph widget).

## DcDemo widget

Tabs **DC** | **Board**. Send → `SendCommand`, Read speed → `GetSpeed`.

## Adc widget

`LinkedFirmataName`, pin selection by label (`UArduinoPinMap`), `UseLinkedAnalogSamples` (read `AnalogSamples` from linked Firmata) or `ReadAdcFlag` for one-shot.

## Resources

- `GUI/Qt/Resources/hardware_lib.qrc`
- Disk fallback under `NMSDK_ROOT` for dev without qrc rebuild

## Build

Requires Qt **Svg**. Without Svg — CMake warning, diagram unavailable.

## See also

- [Usage-Examples.md](Usage-Examples.md)
- [Architecture.md](Architecture.md) — threading
