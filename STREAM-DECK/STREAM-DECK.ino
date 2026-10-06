#include <DHT.h>

#define DHTPIN 18
#define DHTTYPE DHT11
#define FAN_PIN 25

DHT dht(DHTPIN, DHTTYPE);

int pwm_values[] = {0, 80, 150, 255, 150, 80, 0};
int index_pwm = 0;

unsigned long lastSensorTime = 0;
unsigned long lastFanTime = 0;

const unsigned long sensorInterval = 2000;
const unsigned long fanInterval = 3000;

void setup() {
  Serial.begin(115200);
  delay(1000);

  Serial.println("Prueba ESP32 + DHT11 + Ventilador");
  Serial.println("----------------------------------");

  dht.begin();

  pinMode(FAN_PIN, OUTPUT);
  analogWriteResolution(FAN_PIN, 8);
  analogWrite(FAN_PIN, 0);

  Serial.println("Sistema iniciado");
  Serial.println("Abre el monitor serial a 115200 baudios");
}

void loop() {
  unsigned long now = millis();

  if (now - lastSensorTime >= sensorInterval) {
    lastSensorTime = now;

    float temperatura = dht.readTemperature();
    float humedad = dht.readHumidity();

    if (isnan(temperatura) || isnan(humedad)) {
      Serial.println("Error leyendo DHT11. Revisa VCC, GND y DATA.");
    } else {
      Serial.print("Temperatura: ");
      Serial.print(temperatura);
      Serial.print(" °C | Humedad: ");
      Serial.print(humedad);
      Serial.println(" %");
    }
  }

  if (now - lastFanTime >= fanInterval) {
    lastFanTime = now;

    int pwm = pwm_values[index_pwm];
    analogWrite(FAN_PIN, pwm);

    Serial.print("Ventilador PWM: ");
    Serial.print(pwm);
    Serial.print(" / 255  ->  ");

    if (pwm == 0) {
      Serial.println("apagado");
    } else if (pwm < 120) {
      Serial.println("baja velocidad");
    } else if (pwm < 220) {
      Serial.println("velocidad media");
    } else {
      Serial.println("velocidad maxima");
    }

    index_pwm++;

    if (index_pwm >= 7) {
      index_pwm = 0;
    }
  }
}
