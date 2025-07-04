
#include <Wire.h> 
#include <LiquidCrystal_I2C.h>

LiquidCrystal_I2C lcd(0x27,16,2);
int pin = 2;

void setup()
{
  Wire.begin(21, 22);
  pinMode(pin, HIGH);
  lcd.init();
  Serial.begin(115200);
  lcd.backlight();
}

void loop()
{
  digitalWrite(pin, LOW);
  if (Serial.available()) 
  {
    String msg = Serial.readStringUntil('\n');
    lcd.clear();
    msg.trim();
    lcd.setCursor(0,0);
    lcd.print(msg);
  } else {
    digitalWrite(pin, HIGH);
  }
}
