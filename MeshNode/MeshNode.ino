#include <ArduinoJson.h>
#include <Adafruit_BME280.h>
#include <HardwareSerial.h>
#include <painlessMesh.h>

#define VERBOSE true

#define LED_PIN 2
#define I2C_SDA 26
#define I2C_SCL 27
#define RX2 16
#define TX2 17

#define SEALEVELPRESSURE_HPA (1013.25)
#define BME280_ADD 0x76

#define MESH_PREFIX "mesh"
#define MESH_PASSWORD "password"
#define MESH_PORT 5555

Adafruit_BME280 bme(I2C_SDA, I2C_SCL);
HardwareSerial Serial2(2);
painlessMesh mesh;
Scheduler userScheduler;

bool hasBME = false;

void broadcastSensorData();

Task broadcastSensorDataTask(TASK_SECOND * 60, TASK_FOREVER, &broadcastSensorData);

void broadcastSensorData() {
  bme.takeForcedMeasurement();

  StaticJsonBuffer<500> jsonBuffer;
  JsonObject& root = jsonBuffer.createObject();
  root["node_id"] = mesh.getNodeId();
  root["temperature"] = bme.readTemperature();
  root["pressure"] = bme.readPressure() / 100.0F;
  root["altitude"] = bme.readAltitude(SEALEVELPRESSURE_HPA);
  root["humidity"] = bme.readHumidity();

  String json;
  root.printTo(json);

  Serial.printf("Sending msg=%s\n", json.c_str());
  Serial2.println(json);

  mesh.sendBroadcast(json);
}

void receivedMessage(uint32_t from, String &msg) {
  Serial.printf("Received from %u msg=%s\n", from, msg.c_str());
  Serial2.println(msg);
}

void setup() {
  Serial.begin(115200);
  Serial2.begin(9600, SERIAL_8N1, RX2, TX2);

  hasBME = bme.begin(BME280_ADD);

  if (hasBME) {
    bme.setSampling(
      bme.MODE_FORCED,
      bme.SAMPLING_X1,
      bme.SAMPLING_X1,
      bme.SAMPLING_X1,
      bme.FILTER_OFF
    );
  } else {
    Serial.println("Could not find a valid BME280 sensor.");
  }

  mesh.setDebugMsgTypes(ERROR | STARTUP);

  mesh.init(MESH_PREFIX, MESH_PASSWORD, &userScheduler, MESH_PORT);
  mesh.onReceive(&receivedMessage);

  if (hasBME) {
    userScheduler.addTask(broadcastSensorDataTask);
    broadcastSensorDataTask.enable();
  }
}

void loop() {
  userScheduler.execute();
  mesh.update();
}
