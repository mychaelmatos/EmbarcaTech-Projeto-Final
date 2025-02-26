# Sistema Inteligente de Controle e Monitoramento de Bomba d'Água

Este projeto é um sistema embarcado desenvolvido para a plataforma Raspberry Pi Pico que realiza o controle automático de uma bomba d'água, monitorando os níveis de água em um poço e um tanque principal.

## Link do vídeo de demonstração e explicação
[Inserir link do vídeo aqui]

## Link para demonstração no simulador WOKWI
[Inserir link do WOKWI aqui]

## Descrição

O sistema monitora continuamente os níveis de água do poço (>10.000L) e do tanque (<2.000L), controlando automaticamente uma bomba d'água com feedback visual através de display OLED e matriz de LEDs RGB, além de alertas sonoros.

### Funcionalidades Principais

- Monitoramento automático dos níveis de água
- Controle inteligente da bomba
- Interface visual via display OLED
- Indicadores LED RGB para status do sistema
- Alerta sonoro durante operação da bomba
- Simulação via botões para teste do sistema

## Estrutura do Projeto

- `blink.c`: Código principal do sistema
- `ssd1306.h`: Biblioteca para controle do display OLED
- `ws2812.pio`: Configuração para LEDs RGB WS2812
- Arquivos de configuração do SDK Pico

## Hardware Necessário

- Raspberry Pi Pico
- Display OLED (I2C)
- Matriz LED WS2812 5x5
- Buzzer
- 2 Botões para simulação
- Componentes de conexão

## Como Compilar e Executar

1. Clone o repositório
2. Configure o ambiente de desenvolvimento Pico SDK
3. Execute os comandos de compilação:
   ```bash
   mkdir build
   cd build
   cmake ..
   make
   ```
4. Conecte o Pico em modo BOOTSEL
5. Copie o arquivo .uf2 gerado para o Pico

## Operação do Sistema

- **Botão A**: Simula sensor de nível do poço
- **Botão B**: Simula sensor de nível do tanque
- **Display OLED**: Mostra status do sistema
- **LEDs RGB**:
  - Verde: Nível adequado
  - Vermelho: Nível baixo
  - Azul: Bomba em operação

## Licença

Este projeto é licenciado sob a [MIT License](LICENSE).

---

Projeto desenvolvido para a Residência do CEPEDI em Sistemas Embarcados - Embarcatech.
