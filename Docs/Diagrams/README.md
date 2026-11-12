# Диаграммы Rdk-HardwareLib

## RU

## Актуальные диаграммы (markdown)

| Тема | Файл |
|------|------|
| Иерархия классов, runtime | [Architecture.md](../Architecture.md) |
| Связи компонентов | [Components/](../Components/) (mermaid в каждой странице) |
| Firmata scope | [firmata_spike.md](../firmata_spike.md) |

## Pinout (SVG + JSON)

Интерактивная diagram в GUI (вкладка **Pinout**):

- `GUI/Qt/Resources/boards/arduino_uno_pinout.svg`
- `GUI/Qt/Resources/boards/arduino_mega2560_pinout.svg`
- `GUI/Qt/Resources/boards/uno_pins.json`, `mega2560_pins.json`

## Assembly (схема сборки)

Отдельный слой поверх Catalog/Setup — вкладка **Assembly**, не hit-targets Firmata. См. [Assembly-View.md](../Assembly-View.md). Ассеты shields/modules: `Catalog/assets/`.

См. [GUI.md](../GUI.md).

## Legacy

Исторические sequence/state диаграммы для `UArduinoControl` / `UArduinoConnect` — [Legacy/](../Legacy/).

## Doxygen

UML из исходников (если сгенерировано): `Docs/Doxygen/html/` — см. [README.md](../README.md#doxygen).

---

## EN

## Current diagrams (markdown)

| Topic | File |
|-------|------|
| Class hierarchy, runtime | [Architecture.md](../Architecture.md) |
| Component relationships | [Components/](../Components/) (mermaid on each page) |
| Firmata scope | [firmata_spike.md](../firmata_spike.md) |

## Pinout (SVG + JSON)

Interactive diagram in the GUI:

- `GUI/Qt/Resources/boards/arduino_uno_pinout.svg`
- `GUI/Qt/Resources/boards/arduino_mega2560_pinout.svg`
- `GUI/Qt/Resources/boards/uno_pins.json`, `mega2560_pins.json`

See [GUI.md](../GUI.md).

## Legacy

Historical sequence/state diagrams for `UArduinoControl` / `UArduinoConnect` — [Legacy/](../Legacy/).

## Doxygen

UML from sources (if generated): `Docs/Doxygen/html/` — see [README.md](../README.md#doxygen).
