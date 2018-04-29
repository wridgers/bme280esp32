# bme280esp32

A few projects for doing neat things with the ESP32 and BME280 using [BME280-I2C-ESP32](https://github.com/Takatsuki0204/BME280-I2C-ESP32).

## Projects

### HTTPServer

A simple HTTP server node. Connects to WiFi and exposes sensor data as JSON. Can periodicallty POST sensor data to another HTTP server. See `sensor.py` example.

#### Wiring

![HTTPServer wiring](https://raw.githubusercontent.com/wridgers/bme280esp32/master/HTTPServer/breadboard.jpg)

ESP32 | BME280
----- | ------
3.3V | VCC
GND | GND
GPIO27 | SCL
GPIO26 | SDA
