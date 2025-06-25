#include <WiFi.h>

// 🧠 SET DIFFERENT IPs FOR EACH DEVICE
String ssid = "Harrenhal";
String password = "flat_6b@";

// SET THIS TO THE OTHER ESP32's IP
const char* peerIP = "192.168.0.106";  // <-- change this on each ESP

WiFiServer server(80);
String myName = "ESP32-B";  // <-- change to ESP32-B on the second device

void setup() {
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
  Serial.println("\n\n  " + myName);
  Serial.println("===========");
  Serial.println("Ready to chat...\nWrite something:");
}

void loop() {
  WiFiClient incoming = server.available();
  if (incoming) {
    String msg = incoming.readStringUntil('\n');
    if (msg.length() > 0) {
      Serial.print("Message from ");
      Serial.println(msg);
    }
    incoming.stop();
  }

  if (Serial.available()) {
    String outMsg = Serial.readStringUntil('\n');
    WiFiClient client;
    if (client.connect(peerIP, 80)) {
      client.println(myName + ": " + outMsg);
      client.stop();
      Serial.println("Sent: " + outMsg);
    } else {
      Serial.println("Failed to send message");
    }
  }
}