void setup(){
  Serial.begin(115200);
}

void loop(){
  int air = analogRead(34);

  Serial.print("air Quality : ");
  Serial.println(air);
  delay(1000);
}