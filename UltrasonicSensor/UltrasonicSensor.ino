#define TRIG_PIN 9 //set Trig 
#define ECHO_PIN 10//set Echo

void setup() {
  Serial.begin(9600);//set Serial monitor(9600)
  pinMode(TRIG_PIN, OUTPUT);//trig is output
  pinMode(ECHO_PIN, INPUT);//echo is input
}

void loop() {
  // Send trigger pulse
  digitalWrite(TRIG_PIN, LOW);
  delayMicroseconds(2);
  digitalWrite(TRIG_PIN, HIGH);
  delayMicroseconds(10);
  digitalWrite(TRIG_PIN, LOW);

  // Read echo pulse
  long duration = pulseIn(ECHO_PIN, HIGH);

  // Convert to distance (cm)
  float distance = duration * 0.034 / 2;

  Serial.print("Distance: ");
  Serial.print(distance);
  Serial.println(" cm");

  delay(500);
}