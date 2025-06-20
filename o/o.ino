HardwareSerial camSerial(2); // Use UART2 (GPIO16 = RX2, GPIO17 = TX2)

void setup() {
  Serial.begin(115200);
  camSerial.begin(115200, SERIAL_8N1, 16, 17); // RX, TX
  Serial.println("Sending command to CAM...");
  camSerial.println("HELLO_CAM");
}

void loop() {}