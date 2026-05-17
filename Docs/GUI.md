# GUI — Rdk-HardwareLib.gui

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

- `getProp` / `setProp` — чтение/запись свойств
- `envCalculate` — один тик расчёта
- **`pulseEdge(ctx, "Connect")`** — `setProp(edge, "1")` + `envCalculate` (edge сбрасыется в C++)

Кнопки Connect/Disconnect/Reconnect/Upload **не** вызывают C++ напрямую — только property API.

## Вкладка Board

[`HardwareArduinoBoardPanelWidget`](../GUI/Qt/widgets/HardwareArduinoBoardPanelWidget.cpp) — общая панель:

- Порт, baud, bundled firmware, upload
- Connect / Disconnect / Reconnect / Health (`pulseEdge`)
- Статус: `IsConnected`, `HasError`, `LastError` (read-only)
- **Disconnect не очищает `PortName`**

Встроена во вкладку **Board** у SensorSketch, Firmata, DcDemo.

## Diagram

[`UArduinoBoardDiagramWidget`](../GUI/Qt/widgets/UArduinoBoardDiagramWidget.cpp):

- SVG pinout Uno / Mega (`hardware_lib.qrc`)
- Overlay пинов из `uno_pins.json`, `mega2560_pins.json`
- Firmata: клик по пину → `SelectedPin`

## Sensor sketch widget

Вкладки **Sensor** | **Board**. Sensor: команды (`pulseEdge("SendCommand")`), presets, matrix.

## Firmata widget

Вкладки **Firmata** | **Board**. Restart Firmata → `pulseEdge("RestartFirmata")`. Статус: `IsFirmataReady`, `IsLinkReady`.

## DcDemo widget

Вкладки **DC** | **Board**. Send → `SendCommand`, Read speed → `GetSpeed`.

## Adc widget

`LinkedFirmataName`, analog pin, Read → `ReadAdcFlag`.

## Ресурсы

- `GUI/Qt/Resources/hardware_lib.qrc`
- Fallback с диска при `NMSDK_ROOT` для dev без пересборки qrc

## Сборка

Требуется Qt **Svg**. Без Svg — warning в CMake, diagram недоступен.

## См. также

- [Usage-Examples.md](Usage-Examples.md)
- [Architecture.md](Architecture.md) — threading
