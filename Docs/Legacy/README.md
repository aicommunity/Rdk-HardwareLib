# Legacy documentation

Историческая документация удалённых классов. **Не использовать** для новых проектов.

## Маппинг ClassName

| Устаревшее | C++ (удалён) | Замена |
|------------|--------------|--------|
| `Arduino` | `UArduinoControl` | `ArduinoBoard` + `ArduinoSensorSketch` или `ArduinoFirmata` |
| — | `UArduinoConnect` | `UArduinoSerialSession` + parser (см. [Transport.md](../Transport.md)) |
| `ADC` | `UADC` | `ArduinoAdc` → [Components/ArduinoAdc.md](../Components/ArduinoAdc.md) |
| `DC` | `UDcControlDemo` | `ArduinoDcDemo` → [Components/ArduinoDcDemo.md](../Components/ArduinoDcDemo.md) |

## Миграция XML

```bash
python3 Scripts/migrate_arduino_classnames.py path/to/configs
```

## Файлы в этой папке

| Файл | Было |
|------|------|
| [Arduino.md](Arduino.md) | `UArduinoControl` |
| [ArduinoConnect.md](ArduinoConnect.md) | `UArduinoConnect` |
| [ADC.md](ADC.md) | `UADC` |
| [DC.md](DC.md) | `UDcControlDemo` |

Актуальный каталог: [Component-Catalog.md](../Component-Catalog.md).
