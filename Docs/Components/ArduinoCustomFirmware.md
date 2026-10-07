# ArduinoCustomFirmware — host для protocol plugin

## RU

Регистрация: `ArduinoCustomFirmware` → `UArduinoCustomFirmware` в [UHardwareLibrary.cpp](../../Core/UHardwareLibrary.cpp). Наследует UArduinoCustomLink; реализует UArduinoPluginHost. [Исходник](../../Core/Links/UArduinoCustomFirmware.cpp) · [Протоколы](../Protocol-Plugins.md).

| Свойство | Default / поведение |
|----------|---------------------|
| HostPluginId | Пусто: plugin выбирается по BundledFirmwareId |
| BundledFirmwareId | sensor_lab_v1 |
| PluginBound | Найден ли зарегистрированный plugin; это не эквивалент ProtocolNegotiated |
| FrameLog | Журнал; удаление старых строк при размере более 4096 символов |
| ClearFrameLog | При Calculate очищает FrameLog и сбрасывается в false |
| NamedValuesJson | JSON object: float keys via `publishNamedFloat`, string keys via `publishNamedString` (напр. `last_line`) |
| DoubleMatrixReadings | Накопленные строки измерений, начальный размер 0×8 |

GUI: Hub Workbench — [GUI.md](../GUI.md). ClDesc Favorites: `Bin/ClDesc/HardwareLibrary/ru-RU/ArduinoCustomFirmware.xml` (worktree `hardware/wheeled-p0`).

При наличии plugin negotiation, binary frames и health-check передаются ему. Без plugin negotiation остаётся false, binary frames пишутся в FrameLog как hex; health-check использует базовую реализацию. enqueueCommand передаёт строку в базовую очередь. publishPinStatusJson в этом классе — no-op. DoubleMatrixReadings накапливает строки без ограничения числа; ClearFrameLog не очищает матрицу.

## EN

Generic host for statically registered protocol plugins, with command queue, frame log, named readings (float + string) and accumulated matrix rows. HostPluginId overrides the bundled firmware id for plugin lookup. PluginBound means lookup succeeded, not that negotiation completed. Without a plugin, binary frames are logged as hex. Clearing FrameLog does not clear accumulated readings. Hub Workbench GUI: see GUI.md.
