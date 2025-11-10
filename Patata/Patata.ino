int patata = 0;
int z = 13;

void setup() {
  Serial.begin(9600);
  pinMode(z, OUTPUT);
}

void loop() {
  Serial.println(patata);
  Serial.println("Patata:");
  patata++;
  digitalWrite(z, HIGH);

  delay(500);
  digitalWrite(z, LOW);
  delay(500);

}
