#include <SPI.h>
#include <MFRC522.h>
/*
SDA - D10
SCK - D13
MOSI - D11
MISO - D12
IRQ - not connected
GND - GND
RST - D9
3.3V - 3.3V
*/

#define RST_PIN 9
#define SS_PIN 10
#define LED_PIN 8

MFRC522 rfid(SS_PIN, RST_PIN);
MFRC522::Uid allowedCard = { 4, { 0x16, 0xDF, 0x57, 0xF9 }, 0 };  // Replace with your card UID

String correctPassword = "1234";
String inputPassword = "";

void setup() {
  Serial.begin(9600);
  SPI.begin();
  rfid.PCD_Init();
  pinMode(LED_PIN, OUTPUT);
  digitalWrite(LED_PIN, LOW);
  Serial.println("Scan your RFID card...");
}

void loop() {
  if (!rfid.PICC_IsNewCardPresent() || !rfid.PICC_ReadCardSerial()) {
    return;
  }
  if (checkUID(rfid.uid)) {
    Serial.println("Valid card! Enter password:");
    inputPassword = "";

    while (true) {
      if (Serial.available()) {
        char c = Serial.read();
        if (c == '\n' || c == '\r') break;
        inputPassword += c;
      }
    }

    if (inputPassword == correctPassword) {
      Serial.println("Access granted ✅");
      digitalWrite(LED_PIN, HIGH);
      delay(5000); // Door stays open
      digitalWrite(LED_PIN, LOW);
    } else {
      Serial.println("Wrong password ❌");
    }
  } else {
    Serial.println("Unauthorized card");
  }
  rfid.PICC_HaltA();
  rfid.PCD_StopCrypto1();
}

void showUID() {
  Serial.print("Scanned UID: ");
  for (byte i = 0; i < rfid.uid.size; i++) {
    Serial.print(rfid.uid.uidByte[i], HEX);
    Serial.print(" ");
  }
  Serial.println();
}

bool checkUID(MFRC522::Uid uid) {
  if (uid.size != allowedCard.size) return false;
  for (byte i = 0; i < uid.size; i++) {
    if (uid.uidByte[i] != allowedCard.uidByte[i]) return false;
  }
  return true;
}