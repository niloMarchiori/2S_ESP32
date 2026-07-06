#include <Arduino.h>
#include <WiFi.h>
#include <PubSubClient.h>
#include <ArduinoJson.h>
#include "config.h"

static const int SENSOR_PIN = 34;

WiFiClient wifiClient;
PubSubClient mqttClient(wifiClient);

QueueHandle_t readingQueue;

struct SensorReading {
  float value;
  uint32_t timestampMs;
};

//leitura ADC bruta do pino SENSOR_PIN (0-4095)
float readSensor() {
  int raw = analogRead(SENSOR_PIN);
  return (float)raw;
}

// TODO (nucleo): implemente a regra de alerta por limiar.
// Deve retornar true quando o valor cruzar ALERT_THRESHOLD.
bool evaluateRule(float value) {
  return false;
}

void publishReading(const SensorReading &reading) {
  return;
}

void publishAlert(const SensorReading &reading) {
  return;
}

void connectWiFi() {
  Serial.printf("Conectando ao WiFi '%s'...\n", WIFI_SSID);
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  while (WiFi.status() != WL_CONNECTED) {
    delay(250);
    Serial.print(".");
  }
  Serial.println("\nWiFi conectado.");
}

// TODO (diferencial): troque por reconexao com backoff em vez de tentativa bloqueante simples.
void connectMQTT() {
  while (!mqttClient.connected()) {
    String clientId = String("esp32-") + String((uint32_t)ESP.getEfuseMac(), HEX);
    Serial.printf("Conectando ao broker MQTT como '%s'...\n", clientId.c_str());
    if (mqttClient.connect(clientId.c_str())) {
      Serial.println("MQTT conectado.");
    } else {
      Serial.printf("Falha (rc=%d). Tentando novamente em 2s.\n", mqttClient.state());
      delay(2000);
    }
  }
}

void samplingTask(void *pvParameters) {
  for (;;) {
    SensorReading reading;
    reading.value = readSensor();
    reading.timestampMs = millis();
    xQueueSend(readingQueue, &reading, portMAX_DELAY);
    vTaskDelay(pdMS_TO_TICKS(SAMPLE_INTERVAL_MS));
  }
}

void setup() {
  Serial.begin(115200);
  analogReadResolution(12);

  connectWiFi();
  mqttClient.setServer(MQTT_BROKER, MQTT_PORT);

  readingQueue = xQueueCreate(10, sizeof(SensorReading));

  xTaskCreate(samplingTask, "sampling", 4096, NULL, 1, NULL);
}

void loop() {
  if (!mqttClient.connected()) {
    connectMQTT();
  }
  mqttClient.loop();

  SensorReading reading;
  if (xQueueReceive(readingQueue, &reading, pdMS_TO_TICKS(100)) == pdTRUE) {
    publishReading(reading);

    if (evaluateRule(reading.value)) {
      publishAlert(reading);
    }
  }
}
