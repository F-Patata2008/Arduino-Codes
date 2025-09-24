int ussr;
void setup() {
  // put your setup code here, to run once:
  pinMode(13, OUTPUT);
  Serial.begin(9600);
  delay(1000);
  Serial.write("El F-Patata y Sofia presentan: \n");

}

void loop() {
  ussr = Serial.read();
  // put your main code here, to run repeatedly:
  if (ussr == 1) {
    Serial.print("PRObando");
  }
  Serial.write("\n");
  digitalWrite(13, HIGH);
  delay(500);
  Serial.write("hola mundo \n");
  digitalWrite(13, LOW);
  delay(500);
}
