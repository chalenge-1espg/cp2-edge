#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <DHT.h>
#include <RTClib.h>
#include <EEPROM.h>

// === Definições de pinos ===
#define DHTPIN 9
#define DHTTYPE DHT22
#define LDR_PIN A0

#define LED_VERDE 4
#define LED_AMARELO 2
#define LED_VERMELHO 1
#define BUZZER 3

#define BOTAO_TELA 7
#define BOTAO_CONFIG 6

// === Objetos ===
DHT dht(DHTPIN, DHTTYPE);
LiquidCrystal_I2C lcd(0x27, 16, 2);
RTC_DS3231 rtc;

// === Variáveis de sensores ===
const int numAmostras = 5;
float tempAmostras[numAmostras];
float umidAmostras[numAmostras];
float luzAmostras[numAmostras];
int indice = 0;
int totalAmostras = 0;

// === Limites ===
float LIMITE_TEMP = 30.0;
float LIMITE_UMID = 70.0;
float LIMITE_LUZ = 80.0;

// === EEPROM ===
#define END_TEMP 0
#define END_UMID 4
#define END_LUZ 8
#define END_LOGS 12
#define MAX_LOGS 20

int logIndex = 0;

// === Temporizadores ===
unsigned long ultimoMillisLeitura = 0;
unsigned long ultimoMillisLCD = 0;
unsigned long ultimoBotao = 0;
const unsigned long intervaloLeitura = 1000;
const unsigned long intervaloLCD = 3000;
const unsigned long TEMPO_RETORNO_TELA = 10000;

// === Telas ===
int telaAtual = 0;
const int totalTelas = 4;

// === Funções EEPROM ===
void salvarLimitesEEPROM() {
  EEPROM.put(END_TEMP, LIMITE_TEMP);
  EEPROM.put(END_UMID, LIMITE_UMID);
  EEPROM.put(END_LUZ, LIMITE_LUZ);
}

void carregarLimitesEEPROM() {
  EEPROM.get(END_TEMP, LIMITE_TEMP);
  EEPROM.get(END_UMID, LIMITE_UMID);
  EEPROM.get(END_LUZ, LIMITE_LUZ);

  if (isnan(LIMITE_TEMP) || LIMITE_TEMP == 0) LIMITE_TEMP = 30.0;
  if (isnan(LIMITE_UMID) || LIMITE_UMID == 0) LIMITE_UMID = 70.0;
  if (isnan(LIMITE_LUZ) || LIMITE_LUZ == 0) LIMITE_LUZ = 80.0;
}

// === Logs na EEPROM ===
void salvarLogEEPROM(bool tempAlta, bool umidAlta, bool luzAlta) {
  if (!tempAlta && !umidAlta && !luzAlta) return;

  DateTime agora = rtc.now();
  int endereco = END_LOGS + logIndex * 8;

  EEPROM.update(endereco, agora.day());
  EEPROM.update(endereco + 1, agora.month());
  EEPROM.update(endereco + 2, agora.hour());
  EEPROM.update(endereco + 3, agora.minute());

  byte status = 0;
  if (tempAlta) status |= 0b00000001;
  if (umidAlta) status |= 0b00000010;
  if (luzAlta) status |= 0b00000100;
  EEPROM.update(endereco + 4, status);

  EEPROM.update(endereco + 5, 0);
  EEPROM.update(endereco + 6, 0);
  EEPROM.update(endereco + 7, 0);

  logIndex = (logIndex + 1) % MAX_LOGS;
}

void lerLogsEEPROM() {
  Serial.println("=== Logs Salvos ===");
  for (int i = 0; i < MAX_LOGS; i++) {
    int endereco = END_LOGS + i * 8;

    byte dia = EEPROM.read(endereco);
    byte mes = EEPROM.read(endereco + 1);
    byte hora = EEPROM.read(endereco + 2);
    byte minuto = EEPROM.read(endereco + 3);
    byte status = EEPROM.read(endereco + 4);

    if (dia == 0xFF && mes == 0xFF) continue;

    Serial.print("Timestamp: ");
    if (dia < 10) Serial.print("0");
    Serial.print(dia);
    Serial.print("/");
    if (mes < 10) Serial.print("0");
    Serial.print(mes);
    Serial.print(" ");
    if (hora < 10) Serial.print("0");
    Serial.print(hora);
    Serial.print(":");
    if (minuto < 10) Serial.print("0");
    Serial.print(minuto);
    Serial.print(" | ");

    if (status & 0b00000001) Serial.print("Temperatura ALTA | ");
    if (status & 0b00000010) Serial.print("Umidade ALTA | ");
    if (status & 0b00000100) Serial.print("Luminosidade ALTA | ");
    Serial.println();
  }
}

// === Animação inicial ===
void animarLogo() {
  lcd.clear();
  
  const char* texto = "HUNTER";
  int tamanho = strlen(texto);
  lcd.setCursor(5, 0);
  for (int i = 0; i < tamanho; i++) {
    lcd.print(texto[i]);
    delay(300);
  }
  
  lcd.setCursor(0, 1);
  for (int i = 0; i < 16; i++) {
    lcd.print((char)255);
    delay(100);
  }
  
  delay(1000);
  lcd.clear();
}

// === Setup ===
void setup() {
  dht.begin();
  lcd.init();
  lcd.backlight();
  Serial.begin(9600);

  if (!rtc.begin()) {
    Serial.println("RTC não encontrado!");
    while (1);
  }

  if (rtc.lostPower()) {
    Serial.println("RTC sem hora. Definindo...");
    rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));
  }

  pinMode(LED_VERDE, OUTPUT);
  pinMode(LED_AMARELO, OUTPUT);
  pinMode(LED_VERMELHO, OUTPUT);
  pinMode(BUZZER, OUTPUT);
  pinMode(BOTAO_TELA, INPUT_PULLUP);
  pinMode(BOTAO_CONFIG, INPUT_PULLUP);

  carregarLimitesEEPROM();
  animarLogo();

  lerLogsEEPROM();
}

// === Loop principal ===
void loop() {
  unsigned long agora = millis();

  if (agora - ultimoMillisLeitura >= intervaloLeitura) {
    ultimoMillisLeitura = agora;

    float temp = dht.readTemperature();
    float umid = dht.readHumidity();
    int luzBruta = analogRead(LDR_PIN);
    float luz = map(luzBruta, 0, 1023, 100, 0);

    tempAmostras[indice] = temp;
    umidAmostras[indice] = umid;
    luzAmostras[indice] = luz;

    indice = (indice + 1) % numAmostras;
    if (totalAmostras < numAmostras) totalAmostras++;
  }

  static bool botaoAnteriorTela = HIGH;
  bool botaoAtualTela = digitalRead(BOTAO_TELA);
  if (botaoAnteriorTela == HIGH && botaoAtualTela == LOW) {
    telaAtual = (telaAtual + 1) % totalTelas;
    ultimoBotao = agora;
    delay(200);
  }
  botaoAnteriorTela = botaoAtualTela;

  if (telaAtual != 0 && agora - ultimoBotao > TEMPO_RETORNO_TELA) {
    telaAtual = 0;
  }

  static bool botaoAnteriorConfig = HIGH;
  bool botaoAtualConfig = digitalRead(BOTAO_CONFIG);
  if (botaoAnteriorConfig == HIGH && botaoAtualConfig == LOW) {
    float temp = dht.readTemperature();
    float umid = dht.readHumidity();
    int luzBruta = analogRead(LDR_PIN);
    float luz = map(luzBruta, 0, 1023, 100, 0);

    LIMITE_TEMP = temp + 2.0;
    LIMITE_UMID = umid + 5.0;
    LIMITE_LUZ = luz + 10.0;

    salvarLimitesEEPROM();

    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Limites salvos!");
    delay(1000);
  }
  botaoAnteriorConfig = botaoAtualConfig;

  if (agora - ultimoMillisLCD >= intervaloLCD && totalAmostras > 0) {
    ultimoMillisLCD = agora;

    float somaTemp = 0, somaUmid = 0, somaLuz = 0;
    for (int i = 0; i < totalAmostras; i++) {
      somaTemp += tempAmostras[i];
      somaUmid += umidAmostras[i];
      somaLuz += luzAmostras[i];
    }

    float mediaTemp = somaTemp / totalAmostras;
    float mediaUmid = somaUmid / totalAmostras;
    float mediaLuz = somaLuz / totalAmostras;

    bool tempAlta = mediaTemp > LIMITE_TEMP;
    bool umidAlta = mediaUmid > LIMITE_UMID;
    bool luzAlta = mediaLuz > LIMITE_LUZ;

    salvarLogEEPROM(tempAlta, umidAlta, luzAlta);

    DateTime agoraRTC = rtc.now();

    Serial.print("Timestamp: ");
    if (agoraRTC.day() < 10) Serial.print("0");
    Serial.print(agoraRTC.day());
    Serial.print("/");
    if (agoraRTC.month() < 10) Serial.print("0");
    Serial.print(agoraRTC.month());
    Serial.print(" ");
    if (agoraRTC.hour() < 10) Serial.print("0");
    Serial.print(agoraRTC.hour());
    Serial.print(":");
    if (agoraRTC.minute() < 10) Serial.print("0");
    Serial.print(agoraRTC.minute());
    Serial.print(" | ");

    if (tempAlta) Serial.print("Temperatura ALTA | ");
    if (umidAlta) Serial.print("Umidade ALTA | ");
    if (luzAlta) Serial.print("Luminosidade ALTA | ");
    if (!tempAlta && !umidAlta && !luzAlta) Serial.print("Todos OK");
    Serial.println();

    lcd.clear();

    if (telaAtual == 0) {
      int sensoresAcima = tempAlta + umidAlta + luzAlta;
      if (sensoresAcima >= 2) {
        digitalWrite(LED_VERDE, LOW);
        digitalWrite(LED_AMARELO, LOW);
        digitalWrite(LED_VERMELHO, HIGH);
        tone(BUZZER, 1000);
        lcd.setCursor(4, 0);
        lcd.print("PERIGO");
      } else if (sensoresAcima == 1) {
        digitalWrite(LED_VERDE, LOW);
        digitalWrite(LED_AMARELO, HIGH);
        digitalWrite(LED_VERMELHO, LOW);
        tone(BUZZER, 1000);
        delay(200);
        noTone(BUZZER);
        delay(300);
        lcd.setCursor(4, 0);
        lcd.print("ALERTA");
      } else {
        digitalWrite(LED_VERDE, HIGH);
        digitalWrite(LED_AMARELO, LOW);
        digitalWrite(LED_VERMELHO, LOW);
        noTone(BUZZER);
        lcd.setCursor(6, 0);
        lcd.print("OK");
      }

      lcd.setCursor(0, 1);
      lcd.print(agoraRTC.day() < 10 ? "0" : ""); lcd.print(agoraRTC.day());
      lcd.print("/");
      lcd.print(agoraRTC.month() < 10 ? "0" : ""); lcd.print(agoraRTC.month());
      lcd.print(" ");
      lcd.print(agoraRTC.hour() < 10 ? "0" : ""); lcd.print(agoraRTC.hour());
      lcd.print(":");
      lcd.print(agoraRTC.minute() < 10 ? "0" : ""); lcd.print(agoraRTC.minute());

    } else if (telaAtual == 1) {
      lcd.setCursor(0, 0);
      lcd.print("Temperatura:");
      lcd.setCursor(0, 1);
      lcd.print(mediaTemp, 1);
      lcd.print(" C");
    } else if (telaAtual == 2) {
      lcd.setCursor(0, 0);
      lcd.print("Umidade:");
      lcd.setCursor(0, 1);
      lcd.print(mediaUmid, 0);
      lcd.print(" %");
    } else if (telaAtual == 3) {
      lcd.setCursor(0, 0);
      lcd.print("Luminosidade:");
      lcd.setCursor(0, 1);
      lcd.print((int)mediaLuz);
      lcd.print(" %");
    }
  }
}
