# Teste Prático | Desenvolvedor(a) de Firmware | 2Solve

## Contexto

A 2Solve atua no segmento de IIoT, telemetria e monitoramento de ativos. Dispositivos embarcados instalados em campo amostram continuamente variáveis dos ativos — tais como temperatura, pressão, vibração e nível — e precisam decidir, muitas vezes **na própria borda**, quando uma leitura representa uma condição anormal que merece atenção imediata, sem depender exclusivamente do processamento na nuvem.

Este teste espelha, do lado do dispositivo, o mesmo domínio de negócio dos nossos testes de backend para o time de telemetria: lá, as regras de alerta rodam no servidor a partir dos dados recebidos; aqui, elas rodam **no firmware**, mais perto do sensor.

## O desafio

Implemente um **agente de borda em ESP32/FreeRTOS** que:

* **(a)** amostra periodicamente um sensor simulado;
* **(b)** avalia uma **regra local de alerta por limiar** sobre a leitura;
* **(c)** publica **leituras** e **alertas** via **MQTT**, em tópicos distintos, com payload em **JSON**.

Você recebe um **projeto-esqueleto pronto** (veja "Como executar") que já compila e conecta ao WiFi simulado e ao broker MQTT, com a estrutura de tasks/fila do FreeRTOS montada — restando a você implementar a leitura do sensor, a publicação e a regra (pontos marcados com `// TODO` em `src/main.cpp`). Assim você foca a energia na lógica de firmware, não no encanamento de build/conectividade.

### Núcleo obrigatório (~10h com o esqueleto)

* **Leitura periódica do sensor simulado**, lido a partir do ADC configurado no `diagram.json` do esqueleto.
* **Publicação de leituras** no tópico `.../telemetry`, em JSON contendo ao menos o valor lido e um timestamp.
* **Regra de alerta por limiar**: ao a leitura cruzar o limite configurado, publicar um alerta no tópico `.../alerts` (payload JSON).
* **Uso mínimo de FreeRTOS**: a amostragem do sensor deve rodar separada da conectividade/publicação, coordenadas por **task(s) + fila** (o esqueleto já cria parte dessa estrutura — complete o que faltar).

## Restrições técnicas

* **Plataforma:** ESP32 + **FreeRTOS** (via framework Arduino, conforme o esqueleto). O agente deve ser genuinamente multitarefa na parte de amostragem/publicação — não é exigida uma arquitetura elaborada, mas o *super-loop* fazendo tudo sequencialmente não atende ao núcleo obrigatório.
* **Linguagem:** C++.
* **Execução:** o projeto roda no **simulador Wokwi**, seja pela **extensão do VS Code** (licença gratuita, recomendado) ou pelo site **wokwi.com**. Hardware físico é opcional — grave em uma placa real e inclua evidência (log/foto/vídeo) apenas como bônus; isso nunca substitui a verificação em simulação e sua ausência não prejudica a avaliação.
* **Broker MQTT:** broker público de teste (ex.: HiveMQ público ou `test.mosquitto.org`), acessível pelo Public Gateway gratuito do Wokwi. **Defina um prefixo de tópico único** (seu nome, por exemplo) em `include/config.h` para não misturar suas mensagens com as de outros candidatos.
* O código entregue deve estar **plenamente funcional**. A avaliação será conduzida pela execução do material recebido.

## Como executar (Wokwi no VS Code)

1. Crie o **seu repositório a partir do template** e clone-o localmente (veja "Entrega").
2. Instale a extensão **Wokwi for VS Code** (Marketplace).
3. Ative a **licença gratuita** (30 dias, renovável) seguindo o fluxo da própria extensão — não é necessário cartão de crédito nem licença paga.
4. Abra a pasta do projeto no VS Code e edite `include/config.h`, definindo seu `TOPIC_PREFIX` único.
5. Compile com o PlatformIO (`PlatformIO: Build` ou `pio run`).
6. Rode `Wokwi: Start Simulator` (paleta de comandos). O ESP32 simulado conecta ao WiFi `Wokwi-GUEST` via **Public Gateway** (habilitado por padrão, sem custo) e alcança o broker público.
7. Observe as mensagens chegando com um cliente MQTT, por exemplo:
   * `mosquitto_sub -h <broker> -t '<seu-prefixo>/#' -v`
   * ou o cliente web do HiveMQ (`http://www.hivemq.com/demos/websocket-client/`).

Alternativa sem VS Code: importe os arquivos do projeto em [wokwi.com](https://wokwi.com) e rode diretamente pelo navegador.

## O que será avaliado

Pense fora da caixa. O código a IA escreve.

## Entrega

A entrega é feita a partir de um **repositório-template** no GitHub:

1. Acesse o repositório-esqueleto informado pelo recrutador e clique em **"Use this template" → "Create a new repository"** para gerar o **seu próprio repositório**, já com o esqueleto como commit inicial.
2. Desenvolva no seu repositório, versionando o trabalho com Git ao longo do caminho — commits atômicos e **Conventional Commits** são valorizados.
3. Garanta que o repositório final contenha o código-fonte completo (`src/`, `include/`), `diagram.json`, `wokwi.toml` e um **`README.md`** com:
   * seu `TOPIC_PREFIX` e o broker usado, para que o avaliador consiga assinar os tópicos certos;
   * qualquer instrução adicional além do fluxo padrão do esqueleto (se aplicável);
   * (bônus) o documento de projeto mínimo, como seção do README ou arquivo à parte.
4. Envie ao recrutador o **link do seu repositório**. Ele pode ser público, ou privado com o recrutador adicionado como colaborador.

> A avaliação será feita clonando o seu repositório e executando o material recebido; garanta que o fluxo de "clonar → abrir no Wokwi → ver o agente publicando" funcione de ponta a ponta.

## Prazo

* **Início**: imediato.
* **Fim**: segunda-feira, 07h00, horário de Brasília.

Entregas posteriores ao prazo não serão consideradas.
