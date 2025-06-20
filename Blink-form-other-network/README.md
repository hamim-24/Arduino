# 🔥 ESP32 Firebase LED Controller

Control LEDs connected to your ESP32 board remotely using Firebase Realtime Database. This project demonstrates how to sync ESP32 GPIOs with cloud-based state in real time.

---

## 📦 Features

- 🔌 WiFi-connected ESP32
- ☁️ Realtime control via Firebase
- 🔄 Secure login with Firebase email/password
- 💡 Controls two LEDs (GPIO 2 & GPIO 4)
- 📈 Uses `Firebase_ESP_Client` for optimized, stable connection

---

## 🧰 Hardware Requirements

- ESP32 Dev Module or ESP32-CAM
- 2x LEDs with resistors
- Breadboard + jumper wires
- Stable 5V power supply

---

## 🔧 Pin Configuration

| Pin | Purpose       |
|-----|---------------|
| 2   | LED Control 1 |
| 4   | LED Control 2 |

---

## 🌐 Firebase Setup

1. Go to [Firebase Console](https://console.firebase.google.com/)
2. Create a new project
3. Enable **Email/Password** authentication in Auth > Sign-in method
4. Add a **Realtime Database** and set it to test mode
5. Create this path in the database:
   ```json
   {
     "led": {
       "state": 0
     }
   }
   ```
---
## Update Your Code
```cpp
    #define WIFI_SSID ""
    #define WIFI_PASSWORD ""

    #define API_KEY ""

    #define DATABASE_URL "" 

    #define USER_EMAIL ""
    #define USER_PASSWORD ""
```   
---