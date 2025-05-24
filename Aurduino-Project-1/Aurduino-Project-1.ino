#include <SPI.h>
#include <MFRC522.h>

#include <Servo.h>

#include <Wire.h>
#include <LiquidCrystal_I2C.h>

#define CARD_READER_RST 9
#define CARD_READER_SDA 10

#define ULTRASONIC_TRIG 8
#define ULTRASONIC_ECHO 7

#define PIN_RED 5
#define PIN_GREEN 4
#define PIN_SOUND 2

String correctPwd = "1234";
String inputPwd = "";
int pos = 0;
bool rotateDirection = true;

LiquidCrystal_I2C led(0x27, 16, 2);
Servo servo;
Servo servo1;
MFRC522 rfid(CARD_READER_SDA, CARD_READER_RST);
MFRC522::Uid allowedCard = { 4, { 0x16, 0xDF, 0x57, 0xF9 }, 0 };

byte angryFace[8] = {
  0b00000,
  0b10001,
  0b01110,
  0b00000,
  0b00000,
  0b01110,
  0b10001,
  0b00000
};
byte happyFace[8] = {
  0b00000,
  0b01010,
  0b01010,
  0b00000,
  0b00000,
  0b10001,
  0b01110,
  0b00000
};

void setup() {
  Serial.begin(9600);
  servo.attach(6);
  servo1.attach(3);
  servo.write(90);
  servo1.write(70);
  led.init();
  //led.backlight();
  SPI.begin();
  rfid.PCD_Init();
  pinMode(ULTRASONIC_TRIG, OUTPUT);
  pinMode(ULTRASONIC_ECHO, INPUT);
  pinMode(PIN_RED, OUTPUT);
  pinMode(PIN_GREEN, OUTPUT);
  pinMode(PIN_SOUND, OUTPUT);
  led.createChar(0, angryFace);
  led.createChar(1, happyFace);
}

void loop() {

  //checkServo();
  led.noBacklight();
  digitalWrite(PIN_RED, LOW);
  while (!detectObject()) {
    if (pos == 180) rotateDirection = false;
    if (pos == 0) rotateDirection = true;
    if (rotateDirection) {
      servo.write(pos++);
    } else {
      servo.write(pos--);
    }
    delay(5);
  }
  digitalWrite(PIN_RED, HIGH);
  led.backlight();
  led.clear();

  led.setCursor(0, 0);
  led.write(byte(0));
  led.setCursor(2, 0);
  led.print("Who are you?");
  led.setCursor(15, 0);
  led.write(byte(0));

  led.setCursor(0, 1);
  led.print("Scan for Verify");
  Serial.println("Scan for Verify");
  delay(4000);

  if (!rfid.PICC_IsNewCardPresent() || !rfid.PICC_ReadCardSerial()) {
    return;
  }
  if (checkUID(rfid.uid)) {
    led.clear();
    led.setCursor(0, 0);
    led.print("**Valid card**");
    led.setCursor(0, 1);
    led.print("Enter password : ");
    Serial.println("Valid card!\n Enter password : ");
    digitalWrite(PIN_GREEN, HIGH);
    digitalWrite(PIN_RED, LOW);
    inputPwd = "";
    while (true) {
      if (Serial.available()) {
        char c = Serial.read();
        if (c == '\n' || c == '\r') break;
        inputPwd += c;
      }
    }

    if (inputPwd == correctPwd) {
      led.clear();
      led.setCursor(0, 0);
      led.print("**Access granted**");
      led.setCursor(0, 1);
      led.print("**Gate opened**");
      Serial.println("Access granted");
      digitalWrite(PIN_GREEN, HIGH);
      digitalWrite(PIN_RED, LOW);
      delay(2000);
      servo1.write(0);
      for (int i = 0; i < 6; i++) {
        led.clear();
        led.setCursor(0, 0);
        led.print("**Gate opened**");
        led.setCursor(0, 1);
        led.print("Closed at : ");
        led.print(5 - i);
        beep(100);
        delay(1000);
      }
      servo1.write(70);
      beep(1000);
      led.clear();
      led.setCursor(0, 0);
      led.print("**Gate closed**");
      delay(2000);

      digitalWrite(PIN_GREEN, LOW);
      digitalWrite(PIN_RED, HIGH);
    } else {
      digitalWrite(PIN_RED, HIGH);
      digitalWrite(PIN_GREEN, LOW);
      led.clear();
      led.setCursor(0, 0);
      led.print("*Wrong password*");
      Serial.println("**Wrong password**");
      beep(1000);
      delay(2000);
    }
  } else {
    led.clear();
    led.setCursor(0, 0);
    led.print("**Unauthorized**");
    Serial.println("Unauthorized card");
    beep(1000);
    delay(2000);
  }
  rfid.PICC_HaltA();
  rfid.PCD_StopCrypto1();

  led.clear();
  led.noBacklight();
  pos = servo.read();
}

bool checkUID(MFRC522::Uid uid) {
  if (uid.size != allowedCard.size) return false;
  for (byte i = 0; i < uid.size; i++) {
    if (uid.uidByte[i] != allowedCard.uidByte[i]) return false;
  }
  return true;
}

void beep(int i) {
  digitalWrite(PIN_SOUND, HIGH);
  delay(i);
  digitalWrite(PIN_SOUND, LOW);
}

void readAngle() {
  int angle = servo.read();
  Serial.println(angle);
}

void checkServo() {
  led.setCursor(0, 0);
  led.backlight();
  if (!servo.attached()) led.print("Servo not connected");
}

bool detectObject() {
  bool flag = false;
  if (measureDistance() < 20) flag = true;
  return flag;
}

float measureDistance() {
  digitalWrite(ULTRASONIC_TRIG, LOW);
  delayMicroseconds(2);
  digitalWrite(ULTRASONIC_TRIG, HIGH);
  delayMicroseconds(6);
  digitalWrite(ULTRASONIC_TRIG, LOW);

  long duration = pulseIn(ULTRASONIC_ECHO, HIGH);
  float distance = duration * 0.034 / 2;
  return distance;
}