
int temp;

void setup() {
  Serial.begin(115200);
  delay(1000);  // give me time to bring up serial monitor
  Serial.println("ESP32 Touch Test");
  pinMode(2, OUTPUT);
}

void loop() {
  temp = touchRead(T0);
  Serial.println(temp);  // get value using T0
  if (temp <= 30) {
    digitalWrite(2, HIGH);
  }
  else {
    digitalWrite(2, LOW);
  }
  delay(1000);
}
