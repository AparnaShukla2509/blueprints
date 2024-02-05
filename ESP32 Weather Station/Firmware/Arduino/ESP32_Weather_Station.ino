#include <Wire.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BME280.h>
#include <WiFi.h>
#include <PubSubClient.h>
const char* ssid = "Your WiFi SSID";
const char* password = "Your WiFi password";
const char* mqtt_server = "Your MQTT broker IP address";
const char* mqtt_username = "Your MQTT broker username";
const char* mqtt_password = "Your MQTT broker password";
const int mqtt_port = 1883;
Adafruit_BME280 bme;
float temperature;
float humidity;
float pressure;
WiFiClient espClient;
PubSubClient client(espClient);
void connectWiFi() {
  Serial.print("Connecting to WiFi...");
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("WiFi connected!");
}
void connectMQTT() {
  Serial.print("Connecting to MQTT broker...");
  while (!client.connected()) {
    if (client.connect("ESP32Client", mqtt_username, mqtt_password)) {
      Serial.println("MQTT connected!");
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" retrying in 5 seconds");
      delay(5000);
    }
  }
}
void publishData() {
  temperature = bme.readTemperature();
  humidity = bme.readHumidity();
  pressure = bme.readPressure() / 100.0F;
  String payload = "{\"temperature\": " + String(temperature) + ", \"humidity\": " + String(humidity) + ", \"pressure\": " + String(pressure) + "}";
  client.publish("weather/forecast", payload.c_str());
}
void setup() {
  Serial.begin(9600);
  if (!bme.begin()) {
    Serial.println("Could not find a valid BME280 sensor, check wiring!");
    while (1);
  }
  connectWiFi();
  client.setServer(mqtt_server, mqtt_port);
  connectMQTT();
}
void loop() {
  if (WiFi.status() != WL_CONNECTED) {
    connectWiFi();
  }
  if (!client.connected()) {
    connectMQTT();
  }
  publishData();
  delay(300000);
}
