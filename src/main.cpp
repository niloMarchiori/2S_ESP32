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
  static bool alertActive = false;

  if (!alertActive && value >= ALERT_THRESHOLD) {
    alertActive = true;
    return true;
  }

  if (alertActive && value <= ALERT_THRESHOLD - ALERT_HYSTERESIS_DELTA) {
    alertActive = false;
  }

  return false;
}

void publishReading(const SensorReading &reading) {
  if (!mqttClient.connected()) {
    return;
  }

  StaticJsonDocument<128> doc;
  doc["value"] = reading.value;
  doc["timestampMs"] = reading.timestampMs;

  char payload[128];
  serializeJson(doc, payload, sizeof(payload));

  String topic = String(TOPIC_PREFIX) + "/telemetry";
  mqttClient.publish(topic.c_str(), payload);
}

void publishAlert(const SensorReading &reading) {
  if (!mqttClient.connected()) {
    return;
  }

  StaticJsonDocument<128> doc;
  doc["value"] = reading.value;
  doc["timestampMs"] = reading.timestampMs;
  doc["alert"] = true;
  doc["threshold"] = ALERT_THRESHOLD;

  char payload[128];
  serializeJson(doc, payload, sizeof(payload));

  String topic = String(TOPIC_PREFIX) + "/alerts";
  mqttClient.publish(topic.c_str(), payload);
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
  unsigned long backoffMs = 1000;
  const unsigned long maxBackoffMs = 32000;

  while (!mqttClient.connected()) {
    String clientId = String("esp32-") + String((uint32_t)ESP.getEfuseMac(), HEX);
    Serial.printf("Conectando ao broker MQTT como '%s'...\n", clientId.c_str());
    if (mqttClient.connect(clientId.c_str())) {
      Serial.println("MQTT conectado.");
      break;
    }

    int state = mqttClient.state();
    unsigned long jitter = backoffMs / 2;
    unsigned long delayMs = backoffMs + (jitter ? random(0, jitter) : 0);

    Serial.printf("Falha (rc=%d). Tentando novamente em %.1fs.\n", state, delayMs / 1000.0);
    delay(delayMs);
    backoffMs = min(backoffMs * 2, maxBackoffMs);
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
