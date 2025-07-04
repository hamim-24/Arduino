#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#define Echo 10
#define Trig 9

LiquidCrystal_I2C lcd(0x27, 20, 4);

void setup() {
  lcd.init();
  lcd.backlight();
  Serial.begin(9600);
  pinMode(Echo, INPUT);
  pinMode(Trig, OUTPUT);
}

void loop() {
  digitalWrite(Trig, LOW);
  delay(20);
  digitalWrite(Trig, HIGH);
  delay(20);
  digitalWrite(Trig, LOW);

  long duration = pulseIn(Echo, HIGH);
  float distance = duration * 0.034 / 2;
  lcd.clear(); 
  lcd.setCursor(0, 0);
  Serial.print(distance);
  lcd.print("Distance Measure");
  lcd.setCursor(0, 1);
  lcd.print("Distance : ");
  lcd.println(duration);
  lcd.print("cm");
  delay(500);
}
