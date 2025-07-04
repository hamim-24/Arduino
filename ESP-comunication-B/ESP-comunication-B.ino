#include <WiFi.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>

#define redLed 5
#define greenLed 4

const char* ssid = "IIT-WIFI";
const char* password = "IIT786445";

LiquidCrystal_I2C lcd(0x27, 20, 4);

WiFiServer server(80);
String myName = "B"; // change this to "A" or "B" on each device

// Dynamic Peer IP
const char* peerIP = "192.168.0.103"; // default (can be overwritten)
String peerIPString = peerIP;

IPAddress localIP;
bool scanning = false;
bool ipSet = false;

String outMsg = "";
String msg = "";

void setup() {
  pinMode(redLed, OUTPUT);
  pinMode(greenLed, OUTPUT);

  Wire.begin(21, 22); // SDA = 21, SCL = 22 (ESP32)
  lcd.init();
  lcd.backlight();

  Serial.begin(115200);
  WiFi.begin(ssid, password);

  Serial.print("Connecting to WiFi(" + String(ssid) + ")");
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(500);
  }

  localIP = WiFi.localIP();

  Serial.print("\nConnected. IP: ");
  Serial.println(localIP);

  server.begin();

  Serial.println("\n\n              ESP-" + myName);
  Serial.println("============================================");
  Serial.println("Ready to chat... Type 'scan' to find peers.");
  Serial.println("Or type an IP (e.g. 192.168.0.103) to connect.");
  Serial.println("Then just type your message!");

  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("ESP-");
  lcd.print(myName);
  lcd.setCursor(0, 1);
  lcd.print("Start Writing :");
}

void loop() {
  // Incoming message
  WiFiClient incoming = server.available();
  if (incoming) {
    msg = incoming.readStringUntil('\n');
    msg.trim();
    if (msg.length() > 0) {
      digitalWrite(greenLed, HIGH);
      delay(200);
      digitalWrite(greenLed, LOW);

      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("In: ");
      lcd.print(msg);
      lcd.setCursor(0, 1);
      lcd.print("Out: ");
      lcd.print(outMsg);

      Serial.println("💬 Received: " + msg);
    }
    incoming.stop();
  }

  // Serial input
  if (Serial.available()) {
    String input = Serial.readStringUntil('\n');
    input.trim();

    // SCAN COMMAND
    if (input == "scan") {
      scanNetwork();
      return;
    }

    // IP SELECTION
    if (input.indexOf('.') != -1 && input.length() >= 7) {
      peerIPString = input;
      ipSet = true;
      Serial.println("📲 New peer IP set to: " + peerIPString);

      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("Peer IP set:");
      lcd.setCursor(0, 1);
      lcd.print(peerIPString);
      return;
    }

    // SEND MESSAGE
    if (!ipSet) {
      Serial.println("⚠️ Please set a peer IP first (type 'scan' or paste IP).");
      return;
    }

    outMsg = input;

    WiFiClient client;
    if (client.connect(peerIPString.c_str(), 80)) {
      digitalWrite(redLed, HIGH);
      delay(200);
      digitalWrite(redLed, LOW);

      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("In: ");
      lcd.print(msg);
      lcd.setCursor(0, 1);
      lcd.print("Out: ");
      lcd.print(outMsg);

      client.println(myName + ": " + outMsg);
      client.stop();

      Serial.println("✅ Sent: " + outMsg);
    } else {
      Serial.println("❌ Failed to connect to " + peerIPString);
    }
  }
}

void scanNetwork() {
  Serial.println("📡 Scanning local network...");
  for (int i = 1; i < 255; i++) {
    IPAddress testIP(localIP[0], localIP[1], localIP[2], i);
    WiFiClient testClient;
    if (testClient.connect(testIP, 80)) {
      Serial.print("✅ Found device at: ");
      Serial.println(testIP);
      testClient.stop();
    }
    delay(20); // Smooth scan
  }
  Serial.println("🔍 Scan complete. Type IP to connect:");
}