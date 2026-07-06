# Agente de Borda MQTT (ESP32/FreeRTOS) — 2Solve

Implementação de um agente embarcado para telemetria e monitoramento de ativos com alerta por limiar local em ESP32.

## Visão geral

O agente:
- Amostra periodicamente um sensor (ADC) via FreeRTOS task
- Avalia uma regra de alerta com histerese
- Publica leituras e alertas via MQTT em tópicos distintos com payloads JSON

## Estrutura

```
.
├── platformio.ini      # ambiente PlatformIO (ESP32, framework Arduino)
├── wokwi.toml           # aponta o Wokwi para o binário gerado pelo PlatformIO
├── diagram.json         # ESP32 + potenciômetro (sensor simulado) num pino ADC
├── include/config.h     # WiFi, broker MQTT, prefixo de tópico, limiar de alerta
└── src/main.cpp         # agente: amostragem (task+fila), publicação, regra
```

## Configuração

| Parâmetro | Valor |
|-----------|-------|
| **TOPIC_PREFIX** | `2solve/firmware-teste/niloMarchiori` |
| **Broker MQTT** | `broker.hivemq.com:1883` |
| **Limiar de alerta (ALERT_THRESHOLD)** | `3000` (escala 0-4095 ADC) |
| **Histerese** | `100` (o alerta desativa a `3000 - 100`) |
| **Intervalo de amostragem** | `2000ms` (2 segundos) |

## Verificação

### Cliente mosquitto_sub

Subscrever todos os tópicos e observar as mensagens:

```bash
mosquitto_sub -h broker.hivemq.com -t '2solve/firmware-teste/niloMarchiori/#' -v
```

Saída esperada para threshold=3000 e histerese=100:
```
2solve/firmware-teste/niloMarchiori/telemetry {"value":2124,"timestampMs":162418}
2solve/firmware-teste/niloMarchiori/telemetry {"value":2845,"timestampMs":164418}
2solve/firmware-teste/niloMarchiori/telemetry {"value":3242,"timestampMs":166418}
2solve/firmware-teste/niloMarchiori/alerts {"value":3242,"timestampMs":166418,"alert":true,"threshold":3000}
2solve/firmware-teste/niloMarchiori/telemetry {"value":3114,"timestampMs":168418}
2solve/firmware-teste/niloMarchiori/telemetry {"value":3921,"timestampMs":170418}
2solve/firmware-teste/niloMarchiori/telemetry {"value":2897,"timestampMs":172418}
2solve/firmware-teste/niloMarchiori/telemetry {"value":3032,"timestampMs":174418}
2solve/firmware-teste/niloMarchiori/alerts {"value":3032,"timestampMs":174418,"alert":true,"threshold":3000}
```

### Cliente web HiveMQ

Acesse http://www.hivemq.com/demos/websocket-client/ e inscreva-se no tópico `2solve/firmware-teste/niloMarchiori/#`.

## Pré-requisitos

* [VS Code](https://code.visualstudio.com/)
* Extensão **PlatformIO IDE** (Marketplace)
* Extensão **Wokwi for VS Code** (Marketplace) — licença **gratuita** (30 dias, renovável)

## Como executar

1. Compile o firmware:
   ```
   pio run
   ```
   (ou pela paleta de comandos do VS Code: `PlatformIO: Build`)

2. Inicie a simulação: paleta de comandos → `Wokwi: Start Simulator`.
   * O ESP32 simulado conecta à rede `Wokwi-GUEST` através do **Public Gateway** do Wokwi e alcança o broker público.

3. Observe as mensagens chegando com `mosquitto_sub` (veja acima) ou pelo cliente web do HiveMQ.

### Alternativa (wokwi.com)

Importe os arquivos deste projeto em [wokwi.com](https://wokwi.com) e rode diretamente pelo navegador.

## Implementação

### Núcleo obrigatório

- ✅ **Leitura periódica do sensor**: ADC bruto do pino 34 (0-4095).
- ✅ **Publicação de leituras**: tópico `/telemetry` com JSON contendo `value` e `timestampMs`.
- ✅ **Regra de alerta por limiar**: com histerese (delta=100) para evitar flapping.
- ✅ **Publicação de alertas**: tópico `/alerts` com evento de alerta quando o limiar é cruzado.
- ✅ **FreeRTOS multitarefa**: `samplingTask` (coleta) separada da `loop` principal (publicação/conectividade).

### Diferencial

- ✅ **Reconexão MQTT com backoff exponencial**: inicial 1s → máx 32s com jitter.


