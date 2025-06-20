#include <SPI.h>
#include <MFRC522.h>

#define RST_PIN 4    // Reset pin
#define SS_PIN  5    // Slave select (SDA)

MFRC522 mfrc522(SS_PIN, RST_PIN);

void setup() {
  Serial.begin(115200);
  SPI.begin();       // Init SPI bus
  mfrc522.PCD_Init(); // Init MFRC522
  Serial.println("Scan RFID Card");
}

void loop() {
  // Look for new cards
  if (!mfrc522.PICC_IsNewCardPresent()) return;
  if (!mfrc522.PICC_ReadCardSerial()) return;

  Serial.print("UID: ");
  Serial.print(getUIDString(mfrc522.uid));
  Serial.println();
  delay(1000);
}
// Function to format RFID UID as string
String getUIDString(MFRC522::Uid uid) {
  String uidStr = "";
  for (byte i = 0; i < uid.size; i++) {
    if (uid.uidByte[i] < 0x10) uidStr += "0"; // Add leading zero
    uidStr += String(uid.uidByte[i], HEX);
    uidStr += " ";
  }
  uidStr.toUpperCase(); // Make it uppercase like a boss
  return uidStr;
}