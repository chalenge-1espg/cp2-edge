#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <DHT.h>

#define DHTPIN 9
#define DHTTYPE DHT22
#define LDR_PIN A0

#define LED_VERDE 4
#define LED_AMARELO 2
#define LED_VERMELHO 1
#define BUZZER 3

DHT dht(DHTPIN, DHTTYPE);
LiquidCrystal_I2C lcd(0x27, 16, 2);

// Número de amostras para média
const int numAmostras = 5;
float tempAmostras[numAmostras];
float umidAmostras[numAmostras];
float luzAmostras[numAmostras];
int indice = 0;
int totalAmostras = 0;

// Limites
const float LIMITE_TEMP = 30.0;
const float LIMITE_UMID = 70.0;
const float LIMITE_LUZ = 80.0;

// Temporizadores com millis
unsigned long ultimoMillisLeitura = 0;
unsigned long ultimoMillisLCD = 0;
const unsigned long intervaloLeitura = 1000;  // 1 segundo
const unsigned long intervaloLCD = 5000;      // 5 segundos

void setup() {
  dht.begin();
  lcd.init();
  lcd.backlight();

  pinMode(LED_VERDE, OUTPUT);
  pinMode(LED_AMARELO, OUTPUT);
  pinMode(LED_VERMELHO, OUTPUT);
  pinMode(BUZZER, OUTPUT);
}

void loop() {
  unsigned long agora = millis();

  // Leitura dos sensores a cada 1 segundo
  if (agora - ultimoMillisLeitura >= intervaloLeitura) {
    ultimoMillisLeitura = agora;

    float temp = dht.readTemperature();
    float umid = dht.readHumidity();
    int luzBruta = analogRead(LDR_PIN);
    float luz = map(luzBruta, 0, 1023, 100, 0); // Mais luz = % maior

    // Armazena nos buffers circulares
    tempAmostras[indice] = temp;
    umidAmostras[indice] = umid;
    luzAmostras[indice] = luz;

    indice = (indice + 1) % numAmostras;
    if (totalAmostras < numAmostras) totalAmostras++;
  }

  // Atualiza o LCD e LEDs a cada 5 segundos
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

    // Exibir no LCD
    lcd.setCursor(0, 0);
    lcd.print("T:");
    lcd.print(mediaTemp, 1);
    lcd.print("C U:");
    lcd.print(mediaUmid, 0);
    lcd.print("% ");

    lcd.setCursor(0, 1);
    lcd.print("Luz:");
    lcd.print((int)mediaLuz);
    lcd.print("%    ");

        // Verificação dos níveis
    bool tempAlta = mediaTemp > LIMITE_TEMP;
    bool umidAlta = mediaUmid > LIMITE_UMID;
    bool luzAlta = mediaLuz > LIMITE_LUZ;

    int sensoresAcima = tempAlta + umidAlta + luzAlta;

    if (sensoresAcima >= 2) {
      // Crítico: dois ou mais sensores acima
      digitalWrite(LED_VERDE, LOW);
      digitalWrite(LED_AMARELO, LOW);
      digitalWrite(LED_VERMELHO, HIGH);
      tone(BUZZER, 1000); // Buzzer constante
    } else if (sensoresAcima == 1) {
      // Alerta: apenas um valor acima
      digitalWrite(LED_VERDE, LOW);
      digitalWrite(LED_AMARELO, HIGH);
      digitalWrite(LED_VERMELHO, LOW);

      // Buzzer com apito intermitente
      tone(BUZZER, 1000);     // frequência 1kHz
      delay(200);             // 200 ms ligado
      noTone(BUZZER);
      delay(400);             // 800 ms desligado
    } else {
      // Tudo normal
      digitalWrite(LED_VERDE, HIGH);
      digitalWrite(LED_AMARELO, LOW);
      digitalWrite(LED_VERMELHO, LOW);
      digitalWrite(BUZZER, LOW);
    }
  }
}
