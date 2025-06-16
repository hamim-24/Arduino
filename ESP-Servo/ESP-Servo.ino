#include <ESP32Servo.h>

Servo myServo;

void setup() {
  Serial.begin(115200);
  myServo.attach(13); // GPIO 13 for servo signal
}

void loop() {
  // Sweep from 0 to 180
  for (int angle = 0; angle <= 180; angle += 10) {
    myServo.write(angle);
    delay(500);
  }

  // Sweep back from 180 to 0
  for (int angle = 180; angle >= 0; angle -= 10) {
    myServo.write(angle);
    delay(500);
  }
}