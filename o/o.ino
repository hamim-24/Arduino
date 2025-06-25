#include <WiFi.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>

const char* ssid = "Harrenhal";
const char* password = "flat_6b@";

WiFiServer server(80);
LiquidCrystal_I2C lcd(0x27, 16, 2);

void setup() {
  Wire.begin(21, 22);
  lcd.init();
  lcd.backlight();

  Serial.begin(115200);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) delay(1000);
  Serial.print("Connected to WiFi. Local host: ");
  Serial.println(WiFi.localIP());

  server.begin();
}

bool flag = false;

void loop() {
  WiFiClient client = server.available();
  if (client) {
    if (flag == false) {
      Serial.println("Client connected");
      flag = true;
    }
    while (client.connected()) {
      if (client.available()) {
        String data = client.readString();
        Serial.println(data);
        if (data == "1") {
          lcd.clear();
          lcd.setCursor(0, 0);
          lcd.print("go out");
        }
        else {
          lcd.clear();
          lcd.setCursor(0, 0);
          lcd.print("no one in");
        }
      }
    }
  }
}