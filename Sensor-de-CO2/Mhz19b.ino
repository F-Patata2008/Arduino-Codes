#include <Wire.h>
#include <LiquidCrystal_I2C.h> // Check library documentation for POSITIVE argument
#include <MHZ.h>

// Define I2C address for the LCD (consult your LCD datasheet)
const int lcd_I2C_address = 0x27; // This is a common address, might need adjustment

// Define pins for MH-Z19 sensor (assuming standard SPI pins)
#define CO2_IN 8
#define MH_Z19_RX A0 // D7
#define MH_Z19_TX A1 // D6

// Create objects for LCD display and MH-Z19 sensor
// Adjust pin connections if needed (remove POSITIVE if not required by your library)
LiquidCrystal_I2C lcd(lcd_I2C_address, 16,2); // Adjust pin connections if needed
MHZ co2(MH_Z19_RX, MH_Z19_TX, CO2_IN, MHZ19B);

void setup() {
  Serial.begin(9600);
  pinMode(CO2_IN, INPUT);
  delay(100);
  Serial.println("MHZ 19B with I2C LCD");

  // Initialize LCD display with I2C address and pin connections
  lcd.init();
  lcd.backlight(); // Turn on backlight (optional)
  lcd.clear();
}



void loop() {
  int ppm_uart = co2.readCO2UART();
  int ppm_pwm = co2.readCO2PWM();

  Serial.println(ppm_pwm);
  Serial.println();


// Adjust cursor position for PPM (PWM) - Might need adjustment based on your LCD size
  lcd.print(1,0);
  lcd.print("PPM (PWM):");
  lcd.setCursor(1, 1); // Adjust cursor position for PPM (PWM) value - Might need adjustment based on your LCD size
  lcd.print(ppm_pwm);
  delay(1000);
  lcd.clear();
}
