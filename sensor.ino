#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <DHT.h>

// Pinos
#define DHTPIN 2
#define DHTTYPE DHT22
#define LDR_PIN A0
#define BTN_JOIA 3
#define BTN_TEMP 4
#define BTN_LUZ 5
#define BTN_UMID 6

// LCD I2C
LiquidCrystal_I2C lcd(0x27, 16, 2);

// Sensores
DHT dht(DHTPIN, DHTTYPE);

// Variáveis
int tela = 0;

// Limites (ajuste conforme necessário)
float tempMin = 18.0, tempMax = 28.0;
float umidMin = 30.0, umidMax = 70.0;
int luzMin = 300, luzMax = 700;

void setup() {
  lcd.init();
  lcd.backlight();

  pinMode(BTN_JOIA, INPUT_PULLUP);
  pinMode(BTN_TEMP, INPUT_PULLUP);
  pinMode(BTN_LUZ, INPUT_PULLUP);
  pinMode(BTN_UMID, INPUT_PULLUP);

  dht.begin();
}

void loop() {
  // Troca de tela pelos botões
  if (digitalRead(BTN_JOIA) == LOW) {
    tela = 0;
  } else if (digitalRead(BTN_TEMP) == LOW) {
    tela = 1;
  } else if (digitalRead(BTN_LUZ) == LOW) {
    tela = 2;
  } else if (digitalRead(BTN_UMID) == LOW) {
    tela = 3;
  }

  // Leitura dos sensores
  float temperatura = dht.readTemperature();
  float umidade = dht.readHumidity();
  int luminosidade = analogRead(LDR_PIN);

  // Mostrar tela correspondente
  lcd.clear();
  switch (tela) {
    case 0:
      mostrarTelaJoia(temperatura, umidade, luminosidade);
      break;
    case 1:
      mostrarTemperatura(temperatura);
      break;
    case 2:
      mostrarLuminosidade(luminosidade);
      break;
    case 3:
      mostrarUmidade(umidade);
      break;
  }

  delay(1000);
}

// Telas individuais
void mostrarTemperatura(float t) {
  lcd.setCursor(0, 0);
  lcd.print("Temperatura:");
  lcd.setCursor(0, 1);
  lcd.print(t);
  lcd.print(" C");
}

void mostrarUmidade(float u) {
  lcd.setCursor(0, 0);
  lcd.print("Umidade:");
  lcd.setCursor(0, 1);
  lcd.print(u);
  lcd.print(" %");
}

void mostrarLuminosidade(int l) {
  lcd.setCursor(0, 0);
  lcd.print("Luminosidade:");
  lcd.setCursor(0, 1);
  lcd.print(l);
}

void mostrarTelaJoia(float t, float u, int l) {
  lcd.setCursor(0, 0);
  lcd.print("Status Geral:");

  int status = avaliarStatus(t, tempMin, tempMax) +
               avaliarStatus(u, umidMin, umidMax) +
               avaliarStatus(l, luzMin, luzMax);

  lcd.setCursor(0, 1);
  if (status == 0) {
    lcd.print("Tudo OK :) 👍");
  } else if (status <= 3) {
    lcd.print("Atenção! 👉");
  } else {
    lcd.print("Problema! 👎");
  }
}

// Avaliação dos níveis
int avaliarStatus(float valor, float min, float max) {
  float margem = (max - min) * 0.1;
  if (valor < min - margem || valor > max + margem) return 2;   // Fora
  else if (valor < min || valor > max) return 1;                // Perto
  return 0;                                                     // OK
}
