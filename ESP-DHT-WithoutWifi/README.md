# 🌡️ ESP32 DHT11 Web Server (Wi-Fi Access Point Mode)

This project turns your **ESP32** into a local web server that shows **Temperature** and **Humidity** readings from a **DHT11** sensor. It doesn't need internet—runs fully offline in **Access Point** mode.

## 🚀 Features

- 📶 ESP32 creates its own Wi-Fi hotspot (AP mode)
- 🌡️ Real-time DHT11 readings (Temperature & Humidity)
- 💻 Clean web dashboard (auto-refreshes every 4s)
- 🎨 Responsive HTML + Font Awesome icons
- 🔒 Local-only access (for testing & demo)

---

## 🛠️ Hardware Required

| Component      | Description          |
|----------------|----------------------|
| ESP32 board    | Wi-Fi + BLE capable  |
| DHT11 sensor   | Temp + Humidity      |
| Jumper wires   | For connections      |
| Breadboard     | Optional             |

### 📌 Wiring

| DHT11 Pin | ESP32 Pin |
|----------|------------|
| VCC      | 3.3V       |
| GND      | GND        |
| DATA     | GPIO 22    |

> You can change the pin in the code using: `DHT dht(26, DHT11);`

---

## 📄 Code Breakdown

### 1. Set Up Access Point

```cpp
WiFi.softAP(ssid, password);
```

## ESP32 creates its own Wi-Fi network with:
-	SSID: ESP32-DHT
-	Password: 12345678

