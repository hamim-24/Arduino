#include <WiFi.h>

const char* ssid = "Harrenhal";
const char* password = "flat_6b@";

String inputIP = "";

void setup() {
  Serial.begin(115200);
  WiFi.begin(ssid, password);
  
  Serial.println("Connecting to WiFi");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\nWiFi Connected ✅");
  Serial.println(WiFi.localIP());
  Serial.println("Enter an IP to check:");
}

void loop() {
  if (Serial.available()) {
    inputIP = Serial.readStringUntil('\n');
    inputIP.trim();

    IPAddress ip;
    if (ip.fromString(inputIP)) {
      Serial.print("Checking IP: ");
      Serial.println(ip);

      WiFiClient client;

      // Try connecting to port 80 (HTTP)
      if (client.connect(ip, 80)) {
        Serial.println("✅ IP is online (port 80 reachable).");
        client.stop();
      } else {
        Serial.println("❌ IP is unreachable or port is closed.");
      }
    } else {
      Serial.println("⚠️ Invalid IP format.");
    }

    Serial.println("\nEnter another IP to check:");
  }
}