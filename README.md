# bme280esp32

A few projects for doing neat things with the ESP32 and BME280 using [BME280-I2C-ESP32](https://github.com/Takatsuki0204/BME280-I2C-ESP32).

## A note on accuracy

Initially the values I got didn't seem accurate. The solution was to read the datasheet[1] which recommends certain configurations of sample/poll rate for different applications. In particular, 3.5.1, for weather monitoring. Excessive polling causes excess heat, which leads to inaccurate temperature readings.

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

# References

1. [BME280 Datasheet](https://ae-bst.resource.bosch.com/media/_tech/media/datasheets/BST-BME280_DS001-12.pdf)
