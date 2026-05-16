# GUI — Rdk-HardwareLib.gui

Статическая библиотека **Rdk-HardwareLib.gui** (CMake target), линкуется из NeuroModeler и NeuroModelerConsole.

## Регистрация форм

[`HardwareLibComponentGuiRegistration.cpp`](../GUI/Qt/HardwareLibComponentGuiRegistration.cpp) вызывает `RegisterHardwareLibComponentGuiForms()`:

| ClassName | formId | Widget |
|-----------|--------|--------|
| `ArduinoBoard` | `hw.arduino.board` | `HardwareArduinoBoardControllerWidget` |
| `ArduinoSensorSketch` | `hw.arduino.sensor_sketch` | `HardwareArduinoSensorSketchControllerWidget` |
| `ArduinoFirmata` | `hw.arduino.firmata` | `HardwareArduinoFirmataControllerWidget` |

`ArduinoAdc` и `ArduinoDcDemo` — без отдельных форм (property grid).

## Доступ к модели

[`HardwareGuiHelpers`](../GUI/Qt/widgets/HardwareGuiHelpers.cpp):

- `MModel_GetComponentPropertyValue` / `Set`
- `MEnv_Reset`, `MEnv_Calculate` для Apply / Calculate

## Diagram

[`UArduinoBoardDiagramWidget`](../GUI/Qt/widgets/UArduinoBoardDiagramWidget.cpp):

- SVG pinout Uno / Mega (`hardware_lib.qrc`)
- Overlay пинов из `uno_pins.json`, `mega2560_pins.json`
- Масштаб с сохранением aspect ratio
- Firmata: клик по пину → `SelectedPin`

## Board widget

- Combo портов (USB сверху, `ttyS*` серым)
- Upload firmware, progress, status log (копируемый текст)
- Connect / Disconnect / Health check

## Sensor sketch widget

- Command line, presets, matrix preview, GetDataFromBuffers / GetPinsInfo

## Firmata widget

- Pin mode, digital write, read analog, interactive diagram

## Ресурсы

- `GUI/Qt/Resources/hardware_lib.qrc`
- Fallback с диска при `NMSDK_ROOT` для dev без пересборки qrc

## Сборка

Требуется Qt **Svg**. Без Svg — warning в CMake, diagram недоступен.

## См. также

- [Usage-Examples.md](Usage-Examples.md)
- [Component-Catalog.md](Component-Catalog.md)
