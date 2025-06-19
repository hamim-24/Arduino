
#include "BluetoothSerial.h"

BluetoothSerial serialBT;

String device_name = "ESP32-BT-Slave";
char cmd;

void setup() {
  Serial.begin(115200);
  serialBT.begin(device_name);
  pinMode(2, OUTPUT);
  Serial.printf("The device with name \"%s\" is started.\nNow you can pair it with Bluetooth!\n", device_name.c_str());
}

void loop() {
  if (serialBT.available()) {
    cmd = serialBT.read();
  }
  if (cmd == '1') {
    digitalWrite(2, HIGH);
  }
  if (cmd == '0') {
    digitalWrite(2, LOW); 
  }
  delay(20); 
}
