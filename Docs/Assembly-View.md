# Assembly View

## RU

Вкладка **Assembly** показывает композитную схему: плата + shields из `stack` + modules из `devices` + проводники. Это **не** точный pinout для hit-testing Firmata.

| Слой | Источник |
|------|----------|
| Pinout (вкладка Pinout) | `UArduinoBoardDiagramWidget` + `*_pins.json` |
| Assembly | `UArduinoAssemblySceneBuilder` → `UArduinoAssemblyViewWidget` |

Редактор: `UArduinoHardwareSetupEditorWidget` (board/firmware, список устройств, Validate, Export SVG). Host: `HardwareArduinoAssemblyTabHost` читает `HardwareSetupPath` / `HardwareSetupJson` / defaults из firmware catalog.

Конфликт пинов подсвечивает провод красным (`PinConflict` из validate).

## EN

The **Assembly** tab shows board + shield stack + modules + wires. Firmata precision pin picking stays on the **Pinout** tab.
