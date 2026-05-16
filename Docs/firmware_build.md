# Firmware build

```bash
arduino-cli core install arduino:avr
arduino-cli lib install "DHT sensor library"
arduino-cli compile --fqbn arduino:avr:uno -o uno \
  Libraries/Rdk-HardwareLib/Firmware/sensor_lab
From repo root (requires network and arduino-cli index):

```bash
./Scripts/build_arduino_firmware.sh
```

Manual copy after `arduino-cli compile`:

cp uno/sensor_lab.ino.hex Libraries/Rdk-HardwareLib/Firmware/sensor_lab/uno.hex
arduino-cli compile --fqbn arduino:avr:mega -o mega \
  Libraries/Rdk-HardwareLib/Firmware/sensor_lab
cp mega/sensor_lab.ino.hex Libraries/Rdk-HardwareLib/Firmware/sensor_lab/mega2560.hex
```

StandardFirmata: compile from Arduino IDE examples or `arduino-cli compile` with StandardFirmata sketch.
