#include <HardwareSerial.h>
#include <HTTPClient.h>
#include <WiFi.h>

#define LED_PIN 2
#define RX2_PIN 16
#define TX2_PIN 17

#define WIFI_SSID "ssid"
#define WIFI_PASS "pass"

#define URL "http://192.168.0.4:8080/"

HardwareSerial Serial2(2);
HTTPClient client;

void setupNetwork() {
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

void setup() {
  Serial.begin(115200);
  Serial2.begin(9600, SERIAL_8N1, RX2_PIN, TX2_PIN);

  pinMode(LED_PIN, OUTPUT);

  setupNetwork();
}

void loop() {
  if (WiFi.status() != WL_CONNECTED){
    setupNetwork();
  }

  String json = Serial2.readStringUntil('\n');

  if (json.length() > 0) {
    Serial.println(json);
    Serial.printf("POST %s\n%s\n", URL, json.c_str());

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
  }
}

