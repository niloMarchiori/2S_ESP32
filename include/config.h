#pragma once

// WiFi simulado do Wokwi (Public Gateway, sem senha)
#define WIFI_SSID "Wokwi-GUEST"
#define WIFI_PASSWORD ""

// Broker MQTT publico de teste
#define MQTT_BROKER "broker.hivemq.com"
#define MQTT_PORT 1883

// TODO: troque por um prefixo UNICO (ex.: seu nome, sem espacos/acentos).
// Evita que suas mensagens se misturem com as de outros candidatos no broker publico.
#define TOPIC_PREFIX "2solve/firmware-teste/<seu-nome>"

// Limiar de alerta (escala bruta do ADC, 0-4095)
#define ALERT_THRESHOLD 3000

// Intervalo de amostragem, em milissegundos
#define SAMPLE_INTERVAL_MS 2000
