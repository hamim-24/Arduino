#include <WiFi.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>

#define redLed 5
#define greenLed 4

// 🧠 SET DIFFERENT IPs FOR EACH DEVICE
String ssid = "Harrenhal";
String password = "flat_6b@";

LiquidCrystal_I2C lcd(0x27, 20, 4); 

// SET THIS TO THE OTHER ESP32's IP
const char* peerIP = "192.168.0.103";  // <-- change this on each ESP

WiFiServer server(80);
String myName = "B";  // <-- change to ESP32-B on the second device

void setup() {
  pinMode(redLed, OUTPUT);
  pinMode(greenLed, OUTPUT);

  Wire.begin(21, 22); // ESP32 I2C pins: SDA = 21, SCL = 22
  lcd.init();         // Must use .init() not .begin()
  lcd.backlight(); 

  Serial.begin(115200);
  WiFi.begin(ssid, password);

  Serial.print("Connecting to WiFi(" + ssid + ")");
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(10);
  }

  Serial.print("\nConnected. IP: ");
  Serial.println(WiFi.localIP());
  

  server.begin();
  Serial.println("\n\n                   ESP-" + myName);
  Serial.println("============================================");
  Serial.println("Ready to chat...\nWrite something:");
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("ESP-");
  lcd.print(myName);
  lcd.setCursor(0, 1);
  lcd.print("Start Writing :");
}

String outMsg = "";
String msg = "";
bool msgIn = false;

void loop() {
  WiFiClient incoming = server.available();
  if (incoming) {
    msg = incoming.readStringUntil('\n');
    msg.trim();
    if (msg.length() > 0) {
      digitalWrite(greenLed, HIGH);
      delay(500);
      digitalWrite(greenLed, LOW);

      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("In:");
      lcd.print(msg);
      lcd.setCursor(0, 1);
      lcd.print("Out:");
      lcd.print(outMsg);

      Serial.print("Message from ESP-");
      Serial.println(msg);
    }
    incoming.stop();
  }

  if (Serial.available()) {
    outMsg = Serial.readStringUntil('\n');
    WiFiClient client;
    if (client.connect(peerIP, 80)) {
      digitalWrite(redLed, HIGH);
      delay(500);
      digitalWrite(redLed, LOW);

      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("In:");
      lcd.print(msg);
      lcd.setCursor(0, 1);
      lcd.print("Out:");
      lcd.print(outMsg);
      
      client.println(myName + ": " + outMsg);
      client.stop();
      Serial.println("Sent: " + outMsg);
    } else {
      Serial.println("Failed to send message");
    }
  }
}