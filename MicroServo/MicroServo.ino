#include <Servo.h>

Servo myServo;

void setup() {
  myServo.attach(6);
  Serial.begin(9600);
}

void loop() {
  
}

void sweep() {
  for (int pos = 0; pos <= 180; pos += 1) { // Sweep 0-180
    myServo.write(pos);
    delay(15);  // Wait for servo to reach the position
  }
  for (int pos = 180; pos >= 0; pos -= 1) { // Sweep back 180-0
    myServo.write(pos);
    delay(15);
  }
}

void readAngle() {
  int angle = myServo.read();
  Serial.println(angle);
}

void checkServo() {
  if (myServo.attached()) Serial.println("Servo connected");
  else Serial.println("Servo not connected");
}

void stopServo() {
  myServo.detach();
}