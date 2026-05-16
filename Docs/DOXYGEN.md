# Doxygen для Rdk-HardwareLib

## Роль

| Источник | Назначение |
|----------|------------|
| `Docs/*.md` | Руководство: компоненты, протокол, GUI, примеры |
| Doxygen (`Core/`) | Справочник по C++ API из `.h` |

Markdown **не** генерируется Doxygen автоматически; не дублируйте длинные описания в комментариях к классам без необходимости.

## Запуск

```bash
cd Libraries/Rdk-HardwareLib/Docs/Doxygen
doxygen Doxyfile
```

Результат: `Docs/Doxygen/html/index.html`.

## INPUT

В `Doxyfile`: `INPUT = ./../../` — корень библиотеки (`Core/`, `GUI/`, `Firmware/`). При необходимости сузить до `../../Core` для более быстрой сборки.

## Согласованность

- ClassName и свойства в `Docs/Components/` совпадают с `Bin/ClDesc/HardwareLibrary/ru-RU/*.xml` и `Docs/ClDescLexicon.json`.
- Тестовые конфиги: `Bin/Configs/SpikeSamples/Hardware/README.md`.
