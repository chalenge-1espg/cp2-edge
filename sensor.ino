#include <DHT.h>

// Pinos
#define DHTPIN 2
#define DHTTYPE DHT22
#define LDR_PIN A0

// Inicializa o DHT
DHT dht(DHTPIN, DHTTYPE);

void setup() {
  Serial.begin(9600);
  dht.begin();
}

void loop() {
  // Leitura do DHT22
  float temperatura = dht.readTemperature();
  float umidade = dht.readHumidity();

  // Leitura do sensor de luminosidade (LDR)
  int luminosidade = analogRead(LDR_PIN);

  // Exibição no monitor serial
  Serial.print("Temperatura: ");
  Serial.print(temperatura);
  Serial.println(" *C");

  Serial.print("Umidade: ");
  Serial.print(umidade);
  Serial.println(" %");

  Serial.print("Luminosidade (LDR): ");
  Serial.println(luminosidade);

  Serial.println("------------------------");

  delay(2000); // Aguarda 2 segundos antes da próxima leitura
}
