#include <Servo.h>

#define TRIG_PIN 5
#define ECHO_PIN 4

Servo servo;

double distance() {
  digitalWrite(TRIG_PIN, LOW);
  delayMicroseconds(2);
  digitalWrite(TRIG_PIN, HIGH);
  delayMicroseconds(10);
  digitalWrite(TRIG_PIN, LOW);

  long duration = pulseIn(ECHO_PIN, HIGH);

  return duration * 0.034 / 2;
}

void setup() {
  Serial.begin(9600);
  servo.attach(6);
  pinMode(TRIG_PIN, OUTPUT);
  pinMode(ECHO_PIN, INPUT);
}

void loop() {
  bool rotateDirection = false;
  int pos = 0;
  while (distance() > 10) {
    if (pos == 180) rotateDirection = false;
    if (pos == 0) rotateDirection = true;

    if (rotateDirection) {
      servo.write(pos++);
    } 
    else {
      servo.write(pos--);
    }
    delay(5);
  }
  int angle = servo.read();

  Serial.print("Enemy detected, 🙉 Danger !!! Angle : ");
  Serial.println(angle);
}
