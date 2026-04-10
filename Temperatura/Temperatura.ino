#include "DHT.h"
#include <Wire.h>
#include <LiquidCrystal_I2C.h>

LiquidCrystal_I2C lcd(0x27, 16, 2);

#define DHTPIN 2      // El pin donde está conectado el DHT22
#define DHTTYPE DHT22 // Define el tipo de sensor como DHT 22

// Inicializa el sensor DHT.
DHT dht(DHTPIN, DHTTYPE);

void setup() {
  Serial.begin(9600);
  Serial.println(F("DHTxx test!"));

  dht.begin(); // Inicializa el sensor DHT

  lcd.init();
  lcd.backlight();
  lcd.clear();
}

void loop() {
  // Espera 2 segundos entre mediciones.
  delay(2000);

  // Leer la temperatura y humedad.
  float h = dht.readHumidity();
  float t = dht.readTemperature();

  // Revisa si alguna lectura falló.
  if (isnan(h) || isnan(t)) {
    Serial.println(F("Failed to read from DHT sensor!"));
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Error de Sensor");
    return; // Sale de la función loop
  }

  // Limpia la pantalla para mostrar los nuevos datos.
  lcd.clear();

  // Muestra la humedad en la primera línea.
  lcd.setCursor(0, 0);
  lcd.print("Hum: ");
  lcd.print(h);
  lcd.print(" %");

  // Muestra la temperatura en la segunda línea.
  lcd.setCursor(0, 1);
  lcd.print("Temp: ");
  lcd.print(t);
  lcd.print((char)223); // Caracter para el símbolo de grado (°)
  lcd.print("C");

  Serial.println(F("TEMP:"));
  Serial.println(t);

  Serial.println(F("HUM:"));
  Serial.println(h);
}
