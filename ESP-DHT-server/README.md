# 🌐 ESP32 Web Server for DHT11 Sensor

Real-time temperature 🌡️ and humidity 💧 monitoring via WiFi — hosted directly on your ESP32 with live HTML dashboard and automatic refresh. Fully mobile-responsive and stylish with FontAwesome icons.

---

## 🚀 Features

- 📡 Hosts a live web page showing DHT11 sensor data
- 🔄 Auto-refreshes every 4 seconds
- 📱 Mobile-friendly, sleek interface
- 🧠 Uses ESP32 built-in WiFi — no extra hardware needed
- 📊 Displays both temperature (°C) and humidity (%)

---

## 🧠 Requirements

- ESP32 Dev Board  
- DHT11 Temperature + Humidity Sensor  
- Arduino IDE with these libraries:
  - `WiFi.h`
  - `WebServer.h`
  - `DHT.h`
  - `ESPmDNS.h`
- Breadboard + jumper wires

---

## 🔌 Circuit Diagram

| DHT11 Pin | ESP32 Pin |
|-----------|-----------|
| VCC       | 3.3V      |
| GND       | GND       |
| DATA      | GPIO 26   |

> 📝 You can change the GPIO pin in this line: `DHT dht(26, DHT11);`

---

## 📦 How It Works

1. Connect ESP32 to WiFi (update your SSID & password).
2. It hosts a web server on port `80`.
3. Web page shows live DHT11 sensor readings.
4. Page auto-refreshes every 4 seconds to stay up to date.

---