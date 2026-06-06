# ArduinoAdc

## Назначение

**ClassName:** `ArduinoAdc`  
**C++:** `UArduinoAdc` : `UNet` (база **не** меняется)  
**Роль:** Чтение ADC через связанный `ArduinoFirmata` (свой serial не открывает).

## Ключевые свойства

| Свойство | Роль |
|----------|------|
| `LinkedFirmataName` | Имя узла `ArduinoFirmata` на canvas |
| `AnalogPin` | Номер пина Firmata |
| `ReadAdcFlag` | **Edge:** запросить чтение |
| `AdcValue` | State: 0–1023 |
| `AdcReadOk` | State: firmata найден и `IsLinkReady` |

`firmata->Calculate()` вызывается **только** из `UArduinoAdc::ACalculate` (поток движка).

## Типичная схема

```mermaid
flowchart LR
  Firmata[ArduinoFirmata]
  Adc[ArduinoAdc]
  Adc -->|LinkedFirmataName| Firmata
```

## GUI

`hw.arduino.adc` — `LinkedFirmataName`, analog pin, `pulseEdge("ReadAdcFlag")`.

## Тестовый конфиг

`Bin/Configs/SpikeSamples/Hardware/04-ArduinoAdc/`

## ClDesc

`Bin/ClDesc/HardwareLibrary/ru-RU/ArduinoAdc.xml`
