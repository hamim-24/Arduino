# 🕒 ESP32 NTP Clock with I2C LCD Display

A real-time internet-connected digital clock built using an ESP32, synced with NTP servers and displayed on an I2C 16x2 LCD. Shows accurate date and time in **Bangladesh Standard Time (BST / UTC+6)**.

---

## 🚀 Features

- 🛰️ Syncs time from NTP servers (`pool.ntp.org`, `time.nist.gov`)
- 📟 Displays current **date and time** on an I2C LCD
- 🌐 Auto-connects to WiFi
- ⏰ Displays time in **Bangladesh Time (BDT / UTC+6)** without DST
- 🧠 Prints logs to Serial Monitor for debugging

---

## 🧠 Requirements

- ESP32 Dev Board  
- I2C 16x2 LCD Display  
- WiFi Network (with internet access)  
- Arduino IDE with:
  - `LiquidCrystal_I2C` library  
  - ESP32 board package installed  

---

## 🔌 Wiring

| LCD Pin | ESP32 Pin         |
|---------|-------------------|
| VCC     | 3.3V or 5V        |
| GND     | GND               |
| SDA     | GPIO 21 (default) |
| SCL     | GPIO 22 (default) |

> 📍 Use an I2C scanner if your LCD address isn’t `0x27`.

---

## 🔧 Configuration

Update these with your WiFi credentials:

```cpp
const char *ssid = "...";
const char *password = "...";
```
Update these with your country:
```cpp
const long gmtOffset_sec = 6 * 3600;     // UTC+6
const int daylightOffset_sec = 0;        // No DST
const char* time_zone = "BDT-6";         // Used with configTime()
```
