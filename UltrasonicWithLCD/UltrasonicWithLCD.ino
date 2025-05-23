#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#define Echo 10
#define Trig 9

LiquidCrystal_I2C lcd(0x27, 16, 2); //creating object

void setup() {
  lcd.init(); //initializing lcd
  lcd.backlight(); // backlight turn on
  pinMode(Echo, INPUT);
  pinMode(Trig, OUTPUT);
}

void loop() {
  digitalWrite(Trig, LOW);
  delay(200);
  digitalWrite(Trig, HIGH);
  delay(200);
  digitalWrite(Trig, LOW);

  long durition = pulseIn(Echo, HIGH);

  lcd.clear();// clearing screen 
  lcd.setCursor(0, 0);
  lcd.print("Distance Measure");
  lcd.setCursor(0, 1);
  lcd.print("Distance : ");
  lcd.print(durition);
  lcd.print("cm");
}
