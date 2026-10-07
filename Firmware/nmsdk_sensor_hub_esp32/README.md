# nmsdk_sensor_hub_esp32_v1

ESP32 twin of `nmsdk_sensor_hub` (same framed v2 wire protocol).

- Baud default **115200**
- Host plugin: `nmsdk_sensor_hub_v1`
- DHT22: `-DDHTTYPE=DHT22`
- DS18B20: `-DNMSDK_SENSOR_HUB_DS18B20=1` + OneWire + DallasTemperature
- Flash: Arduino ESP32 core / esptool (P0 Connect-only from NeuroModeler)
