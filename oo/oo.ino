#include <WiFi.h>

const char* ssid = "Harrenhal";
const char* password = "flat_6b@";

String inputIP = "";

bool isDeviceAlive(IPAddress ip) {
  WiFiClient client;
  const int ports[] = {80, 443, 22}; // Try HTTP, HTTPS, SSH
  for (int i = 0; i < 3; i++) {
    if (client.connect(ip, ports[i])) {
      client.stop();
      return true;
    }
    delay(100); // Short delay between attempts
  }
  return false;
}

void setup() {
  Serial.begin(115200);
  WiFi.begin(ssid, password);

  Serial.print("🔗 Connecting to WiFi");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\n✅ Connected to WiFi!");
  Serial.println("💡 Enter a local IP (e.g., 192.168.0.105):");
}

void loop() {
  if (Serial.available()) {
    inputIP = Serial.readStringUntil('\n');
    inputIP.trim();

    IPAddress ip;
    if (ip.fromString(inputIP)) {
      Serial.print("🔍 Checking IP: ");
      Serial.println(ip);

      if (isDeviceAlive(ip)) {
        Serial.println("✅ Device is alive on the network.");
      } else {
        Serial.println("❌ Device not found or no open ports.");
      }
    } else {
      Serial.println("🚫 Invalid IP format.");
    }

    Serial.println("\n💡 Enter another IP to check:");
  }
}