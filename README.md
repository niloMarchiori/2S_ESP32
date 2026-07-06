# Esqueleto — Agente de Borda MQTT (ESP32/FreeRTOS)

Projeto-base para o teste prático de Firmware da 2Solve. Já compila e conecta ao WiFi
simulado e ao broker MQTT público, com a estrutura de tasks/fila do FreeRTOS montada —
o trabalho do candidato é implementar a leitura do sensor, a publicação e a regra de
alerta (veja os `// TODO` em `src/main.cpp`).

## Estrutura

```
.
├── platformio.ini      # ambiente PlatformIO (ESP32, framework Arduino)
├── wokwi.toml           # aponta o Wokwi para o binário gerado pelo PlatformIO
├── diagram.json         # ESP32 + potenciômetro (sensor simulado) num pino ADC
├── include/config.h     # WiFi, broker MQTT, prefixo de tópico, limiar de alerta
└── src/main.cpp         # agente: amostragem (task+fila), publicação, regra (TODO)
```

## Pré-requisitos

* [VS Code](https://code.visualstudio.com/)
* Extensão **PlatformIO IDE** (Marketplace)
* Extensão **Wokwi for VS Code** (Marketplace) — licença **gratuita** (30 dias, renovável)

## Entrega

Este repositório é um **template**. Para começar:

1. Clique em **"Use this template" → "Create a new repository"** no GitHub para gerar o
   **seu próprio repositório**, já com este esqueleto como commit inicial.
2. Clone o seu repositório e desenvolva nele, versionando com Git ao longo do trabalho
   (commits atômicos e Conventional Commits são valorizados).
3. Ao final, envie ao recrutador o **link do seu repositório** — público, ou privado com
   o recrutador adicionado como colaborador. **Não** abra pull request no repositório-template.

## Passo a passo

1. Abra esta pasta no VS Code.
2. **Edite `include/config.h`** e defina um `TOPIC_PREFIX` único (ex.: seu nome, sem
   espaços/acentos). Isso evita que suas mensagens se misturem com as de outros
   candidatos no broker público.
3. Ative a licença gratuita da extensão Wokwi (siga o fluxo indicado pela própria
   extensão na primeira execução — não exige cartão de crédito).
4. Compile o firmware:
   ```
   pio run
   ```
   (ou pela paleta de comandos do VS Code: `PlatformIO: Build`)
5. Inicie a simulação: paleta de comandos → `Wokwi: Start Simulator`.
   * O ESP32 simulado conecta à rede `Wokwi-GUEST` através do **Public Gateway**
     do Wokwi (habilitado por padrão, sem custo) e alcança a internet real.
   * Em seguida conecta ao broker MQTT público configurado em `config.h`.
6. Observe as mensagens publicadas com um cliente MQTT, por exemplo:
   ```
   mosquitto_sub -h broker.hivemq.com -t '<seu-prefixo>/#' -v
   ```
   ou pelo cliente web do HiveMQ: http://www.hivemq.com/demos/websocket-client/

Alternativa sem VS Code: importe os arquivos deste projeto em
[wokwi.com](https://wokwi.com) e rode diretamente pelo navegador.
