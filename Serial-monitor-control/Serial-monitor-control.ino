
String cmd = "";

void setup() {
  Serial.begin(115200);
  pinMode(2, OUTPUT);
}

void loop() {
  if (Serial.available()) {
    cmd = Serial.readString();
    if (cmd == "on") {
      digitalWrite(2, HIGH);
      Serial.println("light on");
    }
    else if (cmd == "off") {
      digitalWrite(2, LOW); 
      Serial.println("light off");
    }
  }

}
