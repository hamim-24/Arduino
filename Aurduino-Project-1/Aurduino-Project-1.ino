//for RFID
#include <SPI.h>
#include <MFRC522.h>
//for Servo
#include <Servo.h>
//for 16x2 I2C LED
#include <Wire.h>
#include <LiquidCrystal_I2C.h>

// pins for RFID(card reader)
#define CARD_READER_RST 9 
#define CARD_READER_SDA 10
// pins for ultrasonic sensor
#define ULTRASONIC_TRIG 8
#define ULTRASONIC_ECHO 7
// pins for led and bus
#define PIN_RED 5
#define PIN_GREEN 4
#define PIN_SOUND 2

String correctPwd = "1234";// correct pwd
String inputPwd = "";//input pwd
int pos = 0; // motor position
bool rotateDirection = true;// rotation direction

//object for led     (address, colomn, row)
LiquidCrystal_I2C led(0x27, 16, 2);
Servo servo;
Servo servo1;
MFRC522 rfid(CARD_READER_SDA, CARD_READER_RST);
MFRC522::Uid allowedCard = { 4, { 0x16, 0xDF, 0x57, 0xF9 }, 0 };

// for checking UID
// this function is used to see the UID
void showUID() {
  Serial.print("Scanned UID: ");
  for (byte i = 0; i < rfid.uid.size; i++) {
    Serial.print(rfid.uid.uidByte[i], HEX);
    Serial.print(" ");
  }
  Serial.println();
}

//making emoji
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

void setup() {
  Serial.begin(9600);
  servo.attach(6);
  servo1.attach(3);
  servo.write(90);
  servo1.write(70);
  checkServo();
  led.init();
  SPI.begin();
  rfid.PCD_Init();
  pinMode(ULTRASONIC_TRIG, OUTPUT);
  pinMode(ULTRASONIC_ECHO, INPUT);
  pinMode(PIN_RED, OUTPUT);
  pinMode(PIN_GREEN, OUTPUT);
  pinMode(PIN_SOUND, OUTPUT);
  led.createChar(0, angryFace);
}

void loop() {
  //when no oject found
  led.clear();
  led.noBacklight();
  digitalWrite(PIN_RED, LOW);
  while (!detectObject()) {
    // when reached 180 rotate reversr direction
    if (pos == 180) rotateDirection = false;
    // when reached 0 rotate reversr direction
    if (pos == 0) rotateDirection = true;

    if (rotateDirection) {
      servo.write(pos++);
    } 
    else {
      servo.write(pos--);
    }
    delay(5);
  }
  // when detect object
  digitalWrite(PIN_RED, HIGH); // red light on
  led.backlight();
  led.clear();

  led.setCursor(0, 0);
  led.write(byte(0));
  led.setCursor(2, 0);
  led.print("Who are you?");// print on display
  led.setCursor(15, 0);
  led.write(byte(0));

  led.setCursor(0, 1);
  led.print("Scan for Verify");
  Serial.println("Scan for Verify");
  delay(5000);

  //if there’s no new RFID card nearby, or it can’t read its data
  if (!rfid.PICC_IsNewCardPresent() || !rfid.PICC_ReadCardSerial()) {
    return;

  } // if matched
  if (checkUID(rfid.uid)) {
    led.clear();
    led.setCursor(0, 0);
    led.print("**Valid card**");
    led.setCursor(0, 1);
    led.print("Enter password : ");
    Serial.println("**Valid card**\n Enter password : ");
    digitalWrite(PIN_GREEN, HIGH);
    digitalWrite(PIN_RED, LOW);
    inputPwd = "";

    while (true) {
      if (Serial.available()) {
        char c = Serial.read();// reading from serial monitor
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
      digitalWrite(PIN_GREEN, HIGH); // green on
      digitalWrite(PIN_RED, LOW);// red off
      delay(2000);
      servo1.write(0); // open

      for (int i = 0; i < 6; i++) {
        led.clear();
        led.setCursor(0, 0);
        led.print("**Gate opened**");
        led.setCursor(0, 1);
        led.print("Closed at : ");
        Serial.print("Gate close at : ");
        Serial.println(5 - i);
        led.print(5 - i);
        beep(100);
        delay(1000);
      }

      digitalWrite(PIN_GREEN, LOW);// green off
      digitalWrite(PIN_RED, HIGH);// red on

      servo1.write(70); // close

      beep(1000);
      led.clear();
      led.setCursor(0, 0);
      led.print("**Gate closed**");
      Serial.println("**Gate closed**");
      delay(2000);
    } 
    else {
      digitalWrite(PIN_RED, HIGH);// red on
      digitalWrite(PIN_GREEN, LOW);// green off

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
  rfid.PICC_HaltA();// stop scanning
  rfid.PCD_StopCrypto1(); // stop encrypting

  led.clear();
  led.noBacklight(); // turning off the display
  pos = servo.read();// remember the position
}
// checking the UID
bool checkUID(MFRC522::Uid uid) {
  // if length doesn't match 
  if (uid.size != allowedCard.size) return false;
  //checking all the bit
  for (byte i = 0; i < uid.size; i++) {
    if (uid.uidByte[i] != allowedCard.uidByte[i]) return false;
  }
  return true;
}
// for beep sound
void beep(int i) {
  digitalWrite(PIN_SOUND, HIGH);
  delay(i);
  digitalWrite(PIN_SOUND, LOW);
}
//checking the servo connected or not
void checkServo() {
  led.clear();
  led.setCursor(0, 0);
  led.backlight();
  if (!servo.attached()) led.print("Servo not connected");
}
// detecting objecting at 20 cm
bool detectObject() {
  bool flag = false;
  if (measureDistance() < 20) flag = true;
  return flag;
}
//measure distance
float measureDistance() {
  digitalWrite(ULTRASONIC_TRIG, LOW);
  delayMicroseconds(2);
  digitalWrite(ULTRASONIC_TRIG, HIGH);
  delayMicroseconds(6);
  digitalWrite(ULTRASONIC_TRIG, LOW);
  // measure the distance based on time duration
  long duration = pulseIn(ULTRASONIC_ECHO, HIGH);
  float distance = duration * 0.034 / 2;
  return distance;
}