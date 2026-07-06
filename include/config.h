#pragma once

// WiFi simulado do Wokwi (Public Gateway, sem senha)
#define WIFI_SSID "Wokwi-GUEST"
#define WIFI_PASSWORD ""

// Broker MQTT publico de teste
#define MQTT_BROKER "broker.hivemq.com"
#define MQTT_PORT 1883

#define TOPIC_PREFIX "2solve/firmware-teste/niloMarchiori"

// Limiar de alerta (escala bruta do ADC, 0-4095)
#define ALERT_THRESHOLD 3000

// Histerese do alerta (o alerta desativa quando valor <= ALERT_THRESHOLD - ALERT_HYSTERESIS_DELTA)
#define ALERT_HYSTERESIS_DELTA 100

// Intervalo de amostragem, em milissegundos
#define SAMPLE_INTERVAL_MS 2000
