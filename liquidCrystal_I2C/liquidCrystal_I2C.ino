#include <Wire.h>
#include <LiquidCrystal_I2C.h>

LiquidCrystal_I2C lcd(0x27, 16, 2);
void setup() {
  lcd.init();             
  lcd.backlight();        
  lcd.setCursor(0, 0);    
  lcd.setCursor(0, 1);   
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
lcd.write(byte(0)); 
lcd.print(" Hello");
}

void loop() {
}

