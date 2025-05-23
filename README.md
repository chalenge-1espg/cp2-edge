
# 🏗️ Sistema de Monitoramento Ambiental com Arduino

## 📑 Descrição

Este projeto consiste em um sistema de monitoramento ambiental inteligente, desenvolvido com Arduino, capaz de medir **temperatura**, **umidade** e **luminosidade** em tempo real. O sistema fornece **alertas visuais e sonoros**, exibe as informações em um **display LCD I2C** e registra os eventos críticos com **data e hora** na memória **EEPROM**.

## 🚀 Funcionalidades

- ✅ Medição de **temperatura**, **umidade** e **luminosidade**.
- ✅ Exibição das informações em um **display LCD I2C 16x2**.
- ✅ **Alertas visuais** com LEDs (Verde, Amarelo e Vermelho).
- ✅ **Alarme sonoro** com buzzer em situações críticas.
- ✅ **Logs de eventos** com data e hora utilizando o módulo **RTC DS3231**.
- ✅ Configuração de limites diretamente no hardware usando botões.
- ✅ Interface com **múltiplas telas** para visualização dos dados.
- ✅ Animação de logo na inicialização.

## 📦 Componentes Utilizados

| Componente          | Quantidade |
|---------------------|-------------|
| Arduino (Uno, Nano...) | 1         |
| Sensor de Temperatura e Umidade **DHT22** | 1 |
| Sensor de Luminosidade **LDR** + resistor | 1 |
| Display LCD 16x2 com **módulo I2C** | 1 |
| Módulo RTC **DS3231** | 1 |
| LED Verde | 1 |
| LED Amarelo | 1 |
| LED Vermelho | 1 |
| Buzzer (ativo ou passivo) | 1 |
| Botões | 2 |
| Resistores (para LDR e botões) | diversos |
| Protoboard e fios | - |

## 🗺️ Esquemático (Resumo das Ligações)

| Componente | Pino Arduino |
|-------------|---------------|
| DHT22       | 9             |
| LDR         | A0            |
| LED Verde   | 4             |
| LED Amarelo | 2             |
| LED Vermelho| 1             |
| Buzzer      | 3             |
| Botão Tela  | 7 (INPUT_PULLUP) |
| Botão Config| 6 (INPUT_PULLUP) |
| Display LCD | I2C (SDA A4, SCL A5 no Uno) |
| RTC DS3231  | I2C (SDA A4, SCL A5 no Uno) |

## 🧠 Bibliotecas Necessárias

Instale as bibliotecas no Gerenciador de Bibliotecas do Arduino IDE:

- [`Adafruit DHT`](https://github.com/adafruit/DHT-sensor-library)
- [`RTClib`](https://github.com/adafruit/RTClib)
- [`LiquidCrystal_I2C`](https://github.com/johnrickman/LiquidCrystal_I2C)
- Biblioteca padrão `Wire` (já inclusa no Arduino IDE)
- Biblioteca padrão `EEPROM` (já inclusa no Arduino IDE)

## 🔧 Como Usar

1. Clone este repositório ou baixe os arquivos.
2. Abra o arquivo `.ino` na Arduino IDE.
3. Instale as bibliotecas necessárias.
4. Conecte os componentes conforme o esquemático.
5. Faça o upload do código para seu Arduino.
6. O sistema exibirá uma animação de inicialização e, em seguida, a tela padrão com data, hora e status dos sensores.
7. Use:
   - **Botão Tela:** alterna entre as telas de temperatura, umidade, luminosidade e status geral.
   - **Botão Config:** ajusta os limites atuais adicionando uma margem (temperatura +2°C, umidade +5%, luminosidade +10%).

## 💾 Logs

O sistema salva na memória EEPROM os eventos em que **temperatura**, **umidade** ou **luminosidade** estiverem **acima dos limites configurados**, juntamente com a **data e hora**.

Os logs podem ser lidos pela porta Serial ao iniciar o Arduino.

## 🚨 Alertas

- ✅ **LED Verde:** tudo dentro dos limites.
- ⚠️ **LED Amarelo:** um sensor fora dos limites.
- 🔥 **LED Vermelho + Buzzer:** dois ou mais sensores fora dos limites.


## 📸 Imagens do Projeto

-🛠️ **Projeto montado na bancada**

  <img src="https://github.com/user-attachments/assets/1706809a-d974-4b6d-ad05-b84c90bfc793" width="400"/>
  <img src="https://github.com/user-attachments/assets/76f99541-87f0-4197-aa3a-9ffc0bab02ae" width="400"/>
  
-💡 **HUNTERS EM AÇÃO**

 <img src="https://github.com/user-attachments/assets/f5a345cf-bc1f-4e77-b000-4c397ee066d9" width="400"/>

## 👨‍💻 Autor

- Desenvolvido por Lucas de Almeida, André Spindola, João Palmeira, Eduardo Delarissia



## 📜 Licença

Este projeto é de código aberto, sob a licença **MIT**. Sinta-se livre para usar, modificar e compartilhar.
