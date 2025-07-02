#define TRIG_PIN 8 
#define ECHO_PIN 7

void setup() {
  Serial.begin(9600);
  pinMode(TRIG_PIN, OUTPUT);//trig is output
  pinMode(ECHO_PIN, INPUT);//echo is input
}

void loop() {
  digitalWrite(TRIG_PIN, LOW);
  delayMicroseconds(2);
  digitalWrite(TRIG_PIN, HIGH);
  delayMicroseconds(10);
  digitalWrite(TRIG_PIN, LOW);

  long duration = pulseIn(ECHO_PIN, HIGH);

  float distance = duration * 0.034 / 2;

  Serial.print("Distance: ");
  Serial.print(distance);
  Serial.println(" cm");

  delay(500);
}