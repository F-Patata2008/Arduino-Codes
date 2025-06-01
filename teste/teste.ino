// Librería de manejo de pantalla
#include <U8g2lib.h>
// Librería de conexión con dispositivos I2C
#include <Wire.h>
 
// Definición de la pantalla OLED I2C en el programa
U8G2_SH1106_128X64_NONAME_F_HW_I2C u8g2(U8G2_R0, 255, 13, 15);
 
void setup() {
  // Inicializar comunicación serial
  Serial.begin(115200);
  // Inicializar comunicación con la pantalla
  u8g2.begin();
}
 
void loop() {
  // Limpiar datos de la pantalla antes de dibujar
  u8g2.clearBuffer();
  // Elegir la fuente con la que se escribirá
  u8g2.setFont(u8g2_font_pcsenior_8f);
  // Escribir un texto
  u8g2.drawStr(2, 16, "MakerCamp UDD");
  // Enviar datos para mostrarlos
  u8g2.sendBuffer();
  // Actualiza la pantalla cada 16 milisegundos
  delay(16);
}