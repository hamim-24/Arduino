#include <Wire.h>// it defines the pin of led
//download LiquidCrystal I2C by Frank de Brabander
#include <LiquidCrystal_I2C.h>
// --LiquidCrystal_I2C address 0x27, 16 columns, 2 rows--
//creating object of LiquidCrystal_I2C
LiquidCrystal_I2C lcd(0x27, 16, 2);
//SCL in the A5
//SDA in the A4
//VCC must be 5v
//GND in GND


void setup() {
  lcd.init();             // Initialize LCD
  lcd.backlight();        // Turn on backlightz
  //lcd.noBacklight();        // Turn off backlightz
  lcd.setCursor(0, 0);    // First line(x,y)
  lcd.setCursor(0, 1);    // Second line(x,y)
  byte heart[8] = {
  0b00000,
  0b01010,
  0b11111,
  0b11111,
  0b11111,
  0b01110,
  0b00100,
  0b00000
};

lcd.createChar(0, heart);
lcd.setCursor(0, 0);
lcd.write(byte(0));  // Show custom char
  //lcd.clear();  //clear led
  // lcd.home(); // Moves the cursor to top-left (0,0), like a reset.
}

void loop() {
  // You can animate or update here later
}