#include <Wire.h> 
#include <LiquidCrystal_I2C.h>

LiquidCrystal_I2C lcd(0x27,16,2);  // set the LCD address to 0x27 for a 16 chars and 2 line display

String text = "";

void setup()
{
  Serial.begin(115200);
  lcd.init();                      // initialize the lcd 
  lcd.init();
  // Print a message to the LCD.
  lcd.backlight();
  lcd.setCursor(0,0);
  lcd.print("Write in the ");
  lcd.setCursor(0,1);
  lcd.print("Serial Monitor");
}


void loop()
{
    Serial.println("Write : ");
  if (Serial.available()) {
    text = Serial.readString();
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("You Write : ");
    lcd.setCursor(0, 1);
    lcd.print(text);
    Serial.println("Text added");
  }
  delay(1000);
}