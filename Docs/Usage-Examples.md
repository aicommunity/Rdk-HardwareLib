# Примеры использования Rdk-HardwareLib

## RU

Сценарии через XML-модель и GUI NeuroModeler. Имена свойств совпадают с `Parameters_*.xml`.

Тестовые проекты: `Bin/Configs/SpikeSamples/Hardware/`.

## 1. ArduinoBoard — порт и прошивка

Фрагмент из `01-ArduinoBoard/Parameters_00.xml`:

```xml
<Board Class="ArduinoBoard">
  <Parameters>
    <PortName Type="std::string">/dev/ttyACM0</PortName>
    <BaudRate Type="int">57600</BaudRate>
    <BoardProfile Type="int">0</BoardProfile>
    <ConnectOnBuild Type="bool">0</ConnectOnBuild>
    <BundledFirmwareId Type="std::string">sensor_lab_v1</BundledFirmwareId>
    <UploadFirmwareFlag Type="bool">0</UploadFirmwareFlag>
  </Parameters>
</Board>
```

**GUI:** вкладка Board → выбрать порт → **Connect** (`pulseEdge`) или **Upload firmware** (`UploadFirmware`). Проверить `IsConnected`, `UploadProgress` = 100, `UploadLastResult` = `ok`.

**Edge из схемы (без GUI):**

```xml
<Connect Type="bool" PType="257" IoType="17">1</Connect>
```

После Calculate флаг сбрасывается в `0`. Аналогично: `Disconnect`, `Reconnect`, `UploadFirmware`, `SendCommand` (на CustomLink).

**Linux:** группа `dialout` для доступа к `/dev/ttyACM0`.

## 2. ArduinoSensorSketch — sensor_lab

Добавьте `ArduinoSensorSketch` (может быть единственным компонентом с портом или после Board):

```xml
<SensorSketch Class="ArduinoSensorSketch">
  <Parameters>
    <PortName>/dev/ttyACM0</PortName>
    <BaudRate>57600</BaudRate>
    <ConnectOnBuild>1</ConnectOnBuild>
    <GetDataFromBuffers>1</GetDataFromBuffers>
  </Parameters>
</SensorSketch>
```

**GUI presets:** `START READING`, `STOP READING`, `GET STATUS`, `GET PINS INFO`.

После `START READING` и Calculate с `GetDataFromBuffers` — матрица `DoubleMatrixReadings` заполняется (пакеты `0x01`).

## 3. PROTO 2 (опционально)

`ProtocolVersion` = 2 на sketch, переподключение — хост шлёт `PROTO 2`, прошивка отвечает `PROTO OK 2`. См. [Protocol.md](Protocol.md).

Конфиг-пример: `06-ArduinoSensorSketch-Proto2/`.

## 4. ArduinoFirmata — digital pin 13

1. На `ArduinoBoard`: `BundledFirmwareId` = `standard_firmata`, upload.
2. Компонент `ArduinoFirmata`, тот же `PortName`, `BaudRate` 57600, `ConnectOnBuild` true.
3. Calculate → `FirmataReady` = true.
4. **GUI:** клик по D13 на diagram → `SelectedPin` = 13 → Set pin mode / Write digital.

Конфиг: `03-ArduinoFirmata/`.

## 5. ArduinoAdc + ArduinoFirmata

```xml
<Firmata Class="ArduinoFirmata">
  <Parameters>
    <PortName>/dev/ttyACM0</PortName>
    <ConnectOnBuild>1</ConnectOnBuild>
  </Parameters>
</Firmata>
<Adc Class="ArduinoAdc">
  <Parameters>
    <LinkedFirmataName>Firmata</LinkedFirmataName>
    <AnalogPin>0</AnalogPin>
    <ReadAdcFlag>0</ReadAdcFlag>
  </Parameters>
</Adc>
```

`LinkedFirmataName` — **имя узла** Firmata на схеме, не ClassName.

## 6. ArduinoDcDemo + ArduinoSensorSketch

```xml
<SensorSketch Class="ArduinoSensorSketch">
  <Parameters>
    <PortName>/dev/ttyACM0</PortName>
    <ConnectOnBuild>1</ConnectOnBuild>
  </Parameters>
</SensorSketch>
<DcDemo Class="ArduinoDcDemo">
  <Parameters>
    <LinkedSketchName>SensorSketch</LinkedSketchName>
    <Command>START READING</Command>
    <SendCommandFlag>0</SendCommandFlag>
  </Parameters>
</DcDemo>
```

## 7. Миграция старого проекта

```bash
python3 Scripts/migrate_arduino_classnames.py Bin/Configs/MyProject
```

Замены: `Arduino` → `ArduinoSensorSketch`, `PortToConnect` → `PortName`, `ADC` → `ArduinoAdc`, `DC` → `ArduinoDcDemo`.

## 8. Сборка прошивки

```bash
./Scripts/build_arduino_firmware.sh
```

См. [firmware_build.md](firmware_build.md), [Firmware/README.md](../Firmware/README.md).

## См. также

- [Component-Catalog.md](Component-Catalog.md)
- [Components/](Components/)

---

## EN

Scenarios via XML model and NeuroModeler GUI. Property names match `Parameters_*.xml`.

Test projects: `Bin/Configs/SpikeSamples/Hardware/`.

## 1. ArduinoBoard — port and firmware

Fragment from `01-ArduinoBoard/Parameters_00.xml`:

```xml
<Board Class="ArduinoBoard">
  <Parameters>
    <PortName Type="std::string">/dev/ttyACM0</PortName>
    <BaudRate Type="int">57600</BaudRate>
    <BoardProfile Type="int">0</BoardProfile>
    <ConnectOnBuild Type="bool">0</ConnectOnBuild>
    <BundledFirmwareId Type="std::string">sensor_lab_v1</BundledFirmwareId>
    <UploadFirmwareFlag Type="bool">0</UploadFirmwareFlag>
  </Parameters>
</Board>
```

**GUI:** Board tab → select port → **Connect** (`pulseEdge`) or **Upload firmware** (`UploadFirmware`). Check `IsConnected`, `UploadProgress` = 100, `UploadLastResult` = `ok`.

**Edge from diagram (without GUI):**

```xml
<Connect Type="bool" PType="257" IoType="17">1</Connect>
```

After Calculate the flag resets to `0`. Similarly: `Disconnect`, `Reconnect`, `UploadFirmware`, `SendCommand` (on CustomLink).

**Linux:** `dialout` group for access to `/dev/ttyACM0`.

## 2. ArduinoSensorSketch — sensor_lab

Add `ArduinoSensorSketch` (may be the only component with a port or after Board):

```xml
<SensorSketch Class="ArduinoSensorSketch">
  <Parameters>
    <PortName>/dev/ttyACM0</PortName>
    <BaudRate>57600</BaudRate>
    <ConnectOnBuild>1</ConnectOnBuild>
    <GetDataFromBuffers>1</GetDataFromBuffers>
  </Parameters>
</SensorSketch>
```

**GUI presets:** `START READING`, `STOP READING`, `GET STATUS`, `GET PINS INFO`.

After `START READING` and Calculate with `GetDataFromBuffers` — matrix `DoubleMatrixReadings` is filled (`0x01` packets).

## 3. PROTO 2 (optional)

`ProtocolVersion` = 2 on sketch, reconnect — host sends `PROTO 2`, firmware replies `PROTO OK 2`. See [Protocol.md](Protocol.md).

Config example: `06-ArduinoSensorSketch-Proto2/`.

## 4. ArduinoFirmata — digital pin 13

1. On `ArduinoBoard`: `BundledFirmwareId` = `standard_firmata`, upload.
2. Component `ArduinoFirmata`, same `PortName`, `BaudRate` 57600, `ConnectOnBuild` true.
3. Calculate → `FirmataReady` = true.
4. **GUI:** click D13 on diagram → `SelectedPin` = 13 → Set pin mode / Write digital.

Config: `03-ArduinoFirmata/`.

## 5. ArduinoAdc + ArduinoFirmata

```xml
<Firmata Class="ArduinoFirmata">
  <Parameters>
    <PortName>/dev/ttyACM0</PortName>
    <ConnectOnBuild>1</ConnectOnBuild>
  </Parameters>
</Firmata>
<Adc Class="ArduinoAdc">
  <Parameters>
    <LinkedFirmataName>Firmata</LinkedFirmataName>
    <AnalogPin>0</AnalogPin>
    <ReadAdcFlag>0</ReadAdcFlag>
  </Parameters>
</Adc>
```

`LinkedFirmataName` — **node name** of Firmata on the diagram, not ClassName.

## 6. ArduinoDcDemo + ArduinoSensorSketch

```xml
<SensorSketch Class="ArduinoSensorSketch">
  <Parameters>
    <PortName>/dev/ttyACM0</PortName>
    <ConnectOnBuild>1</ConnectOnBuild>
  </Parameters>
</SensorSketch>
<DcDemo Class="ArduinoDcDemo">
  <Parameters>
    <LinkedSketchName>SensorSketch</LinkedSketchName>
    <Command>START READING</Command>
    <SendCommandFlag>0</SendCommandFlag>
  </Parameters>
</DcDemo>
```

## 7. Migrating an old project

```bash
python3 Scripts/migrate_arduino_classnames.py Bin/Configs/MyProject
```

Replacements: `Arduino` → `ArduinoSensorSketch`, `PortToConnect` → `PortName`, `ADC` → `ArduinoAdc`, `DC` → `ArduinoDcDemo`.

## 8. Firmware build

```bash
./Scripts/build_arduino_firmware.sh
```

See [firmware_build.md](firmware_build.md), [Firmware/README.md](../Firmware/README.md).

## See also

- [Component-Catalog.md](Component-Catalog.md)
- [Components/](Components/)
