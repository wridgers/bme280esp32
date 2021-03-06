#include <WiFi.h>
#include <ESPmDNS.h>
#include <FS.h>
#include <AsyncTCP.h>
#include <ESPAsyncWebServer.h>
#include <ArduinoJson.h>
#include <Adafruit_BME280.h>
#include <HTTPClient.h>

#define LED_PIN 2
#define I2C_SDA 26
#define I2C_SCL 27

#define WIFI_SSID "ssid"
#define WIFI_PASS "pass"

#define URL "http://192.168.0.4:8080/"

#define SEALEVELPRESSURE_HPA (1013.25)
#define BME280_ADD 0x76

Adafruit_BME280 bme(I2C_SDA, I2C_SCL);
AsyncWebServer server(80);
HTTPClient client;

void setup_network() {
  digitalWrite(LED_PIN, LOW);

  WiFi.disconnect(true);

  WiFi.begin(WIFI_SSID, WIFI_PASS);
  Serial.printf("Connecting to WiFi network %s ", WIFI_SSID);

  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(1000);
  }

  Serial.print(" Connected!\nIP Address: ");
  Serial.println(WiFi.localIP());

  digitalWrite(LED_PIN, HIGH);
}

String get_json() {
  StaticJsonBuffer<500> jsonBuffer;
  JsonObject& root = jsonBuffer.createObject();
  root["temperature"] = bme.readTemperature();
  root["pressure"] = bme.readPressure() / 100.0F;
  root["altitude"] = bme.readAltitude(SEALEVELPRESSURE_HPA);
  root["humidity"] = bme.readHumidity();

  String json;
  root.printTo(json);

  return json;
}

void setup() {
  Serial.begin(115200);
  pinMode(LED_PIN, OUTPUT);
  digitalWrite(LED_PIN, LOW);

  bool status = bme.begin(BME280_ADD);
  if (!status) {
    Serial.println("Could not find a valid BME280 sensor, check wiring!");
    while (1) {
      delay(1000);
    }
  }

  bme.setSampling(
    bme.MODE_FORCED,
    bme.SAMPLING_X1,
    bme.SAMPLING_X1,
    bme.SAMPLING_X1,
    bme.FILTER_OFF
  );

  setup_network();

  if (!MDNS.begin("kitchen")) {
    Serial.println("Error setting up MDNS responder!");

    while(1) {
      delay(1000);
    }
  }

  MDNS.addService("http", "tcp", 80);

  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request) {
    request->send(200, "application/json", get_json());
  });

  server.begin();
  Serial.println("HTTP server started");
}

void loop() {
  if (WiFi.status() != WL_CONNECTED){
    setup_network();
  }

  bme.takeForcedMeasurement();

  String json = get_json();

  Serial.print("POST ");
  Serial.println(URL);
  Serial.println(json);

  client.begin(URL);
  client.addHeader("Content-Type", "application/json");
  int response_code = client.POST(json);

  if (response_code > 0) {
    String response = client.getString();
    Serial.printf("%d %s\n", response_code, response.c_str());
  } else {
    Serial.println("Failed to send POST");
  }

  client.end();

  delay(60000);
}
