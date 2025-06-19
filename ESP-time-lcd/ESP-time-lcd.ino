#include <WiFi.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include "time.h"
#include "esp_sntp.h"

LiquidCrystal_I2C lcd(0x27, 16, 2);

const char *ssid = "Harrenhal";
const char *password = "flat_6b@";

const char *ntpServer1 = "pool.ntp.org";
const char *ntpServer2 = "time.nist.gov";
const long gmtOffset_sec = 6 * 3600; // +6 hours = 21600
const int daylightOffset_sec = 0;    // Bangladesh does NOT use DST

const char* time_zone = "BDT-6"; // Bangladesh Standard Time (no DST)  // TimeZone rule for Europe/Rome including daylight adjustment rules (optional)

void printLocalTime() {
  struct tm timeinfos;
  lcd.clear();
  if (!getLocalTime(&timeinfos)) {
    Serial.println("No time available (yet)");
    lcd.setCursor(0, 0);
    lcd.print("No time available (yet)");
    return;
  }
  getLocalTime(&timeinfos);

  lcd.setCursor(0, 0);
  lcd.print(&timeinfos, "%B, %d %Y");
  lcd.setCursor(0, 1);
  lcd.print(&timeinfos, "%A %H:%M:%S");
  Serial.println(&timeinfos, "%B %d %Y %A %H:%M:%S");
}

// Callback function (gets called when time adjusts via NTP)
void timeavailable(struct timeval *t) {
  Serial.println("Got time adjustment from NTP!");
  printLocalTime();
}

void setup() {
  Serial.begin(115200);
  lcd.init();
  lcd.backlight();

  lcd.setCursor(0, 0);
  lcd.print("Connecting ");
  Serial.printf("Connecting to %s ", ssid);
  WiFi.begin(ssid, password);

  esp_sntp_servermode_dhcp(1); 

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println(" CONNECTED");
  lcd.setCursor(0, 1);
  lcd.print(" CONNECTED");

  sntp_set_time_sync_notification_cb(timeavailable);
  configTime(gmtOffset_sec, daylightOffset_sec, ntpServer1, ntpServer2);

}

void loop() {
  delay(1000);
  printLocalTime();  // it will take some time to sync time :)
}

