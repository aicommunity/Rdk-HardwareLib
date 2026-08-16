# Arduino setup (Windows)

## RU

## One-time toolchain install

From the repo root or `Bin/Platform/Win`:

```powershell
.\Scripts\setup_arduino_tools.ps1
# optional wrapper:
# Bin\Platform\Win\SetupArduinoTools.bat
```

This downloads `arduino-cli` into `Bin/Platform/Win/ArduinoCLI/`, installs the AVR core into `Bin/Platform/Win/ArduinoData/`, copies `avrdude` to `Bin/Platform/Win/ArduinoTools/`, and builds HEX into `Bin/ArduinoFirmware/`.

Run **NeuroModeler** from `Bin/Platform/Win` so upload finds bundled `avrdude` automatically.

## Rebuild firmware only

```powershell
.\Scripts\build_arduino_firmware.ps1 -ArduinoCli "Bin\Platform\Win\ArduinoCLI\arduino-cli.exe" -ArduinoData "Bin\Platform\Win\ArduinoData"
```

## Troubleshooting

| Symptom | Action |
|---------|--------|
| `avrdude not found` | Run `Scripts/setup_arduino_tools.ps1` (or `SetupArduinoTools.bat`), restart NeuroModeler from `Bin/Platform/Win` |
| `avrdude.conf not found` | Check `ArduinoTools/etc/avrdude.conf` exists |
| `HEX file not found` | Re-run setup or `build_arduino_firmware.ps1` |
| COM port busy / permission | Close Arduino IDE Serial Monitor |
| `not in sync` / `not responding` | Verify Board profile (Uno=0, Mega=1), correct COM port; disconnect other components using the same port; retry Upload after closing Serial Monitor |
| `403` on downloads.arduino.cc | Build on another machine; copy `ArduinoTools` and `Bin/ArduinoFirmware` |

## Linux

Use the system `avrdude` package and `./Scripts/build_arduino_firmware.sh`. No `Bin/Platform/Linux/ArduinoTools` is required. See [Transport.md](Transport.md) and [Firmware/README.md](../Firmware/README.md).

---

## EN

## One-time toolchain install

From the repo root or `Bin/Platform/Win`:

```powershell
.\Scripts\setup_arduino_tools.ps1
# optional wrapper:
# Bin\Platform\Win\SetupArduinoTools.bat
```

This downloads `arduino-cli` into `Bin/Platform/Win/ArduinoCLI/`, installs the AVR core into `Bin/Platform/Win/ArduinoData/`, copies `avrdude` to `Bin/Platform/Win/ArduinoTools/`, and builds HEX into `Bin/ArduinoFirmware/`.

Run **NeuroModeler** from `Bin/Platform/Win` so upload finds bundled `avrdude` automatically.

## Rebuild firmware only

```powershell
.\Scripts\build_arduino_firmware.ps1 -ArduinoCli "Bin\Platform\Win\ArduinoCLI\arduino-cli.exe" -ArduinoData "Bin\Platform\Win\ArduinoData"
```

## Troubleshooting

| Symptom | Action |
|---------|--------|
| `avrdude not found` | Run `Scripts/setup_arduino_tools.ps1` (or `SetupArduinoTools.bat`), restart NeuroModeler from `Bin/Platform/Win` |
| `avrdude.conf not found` | Check `ArduinoTools/etc/avrdude.conf` exists |
| `HEX file not found` | Re-run setup or `build_arduino_firmware.ps1` |
| COM port busy / permission | Close Arduino IDE Serial Monitor |
| `not in sync` / `not responding` | Verify Board profile (Uno=0, Mega=1), correct COM port; disconnect other components using the same port; retry Upload after closing Serial Monitor |
| `403` on downloads.arduino.cc | Build on another machine; copy `ArduinoTools` and `Bin/ArduinoFirmware` |

## Linux

Use the system `avrdude` package and `./Scripts/build_arduino_firmware.sh`. No `Bin/Platform/Linux/ArduinoTools` is required. See [Transport.md](Transport.md) and [Firmware/README.md](../Firmware/README.md).
