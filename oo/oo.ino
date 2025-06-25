#include <WiFi.h>

const char* ssid = "Harrenhal";
const char* password = "flat_6b@";

const char* host = "192.168.0.104"; // Replace with Server IP
#define TRIG_PIN 14
#define ECHO_PIN 27

void setup() {
  pinMode(TRIG_PIN, OUTPUT);
  pinMode(ECHO_PIN, INPUT);
  Serial.begin(115200);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) delay(1000);
  Serial.println("Connected to WiFi");
}

void loop() {
  long duration;
  float distanceCm;

  // Trigger the sensor
  digitalWrite(TRIG_PIN, LOW);
  delayMicroseconds(2);
  digitalWrite(TRIG_PIN, HIGH);
  delayMicroseconds(10);
  digitalWrite(TRIG_PIN, LOW);

  // Read echo time
  duration = pulseIn(ECHO_PIN, HIGH);

  // Convert to distance
  distanceCm = duration * 0.0343 / 2;

  WiFiClient client;
  if (client.connect(host, 80)) {
    if(distanceCm < 10) client.print("1");
    else client.print("0");
    // client.println("Yo server ESP32!");
    // String reply = client.readStringUntil('\n');
    // Serial.print("Server says: ");
    // Serial.println(reply);
  } else {
    Serial.println("Connection failed");
  }
  delay(1000); // Wait before retry
}