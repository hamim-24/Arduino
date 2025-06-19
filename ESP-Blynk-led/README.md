# 💡 ESP32 Blynk IoT LED Control

Control your ESP32-connected LED (or any GPIO device) from anywhere in the world using the **Blynk IoT platform**. Fully mobile-based, cloud-connected, and beginner-friendly.

---

## 🚀 Features

- 🌍 Remote control using **Blynk IoT mobile app**
- ⚙️ Real-time LED control from your phone
- 📶 ESP32 WiFi connectivity
- 🔒 Secure cloud-based communication with auth token
- 📱 Cross-platform: Android & iOS support

---

## 🧠 Requirements

- ESP32 Dev Board  
- Blynk IoT Account ([Blynk Console](https://blynk.cloud))  
- Blynk Mobile App (Android/iOS)  
- Internet-connected WiFi  

---

## 📲 Setup Instructions

### 1. 🔗 Create Template in Blynk

- Go to [https://blynk.cloud](https://blynk.cloud)
- Create a new **Template**
- Note down:
  - `BLYNK_TEMPLATE_ID`
  - `BLYNK_TEMPLATE_NAME`
  - `BLYNK_AUTH_TOKEN`

### 2. 📲 Create a Device

- Link your device to the template
- Install the **Blynk App** on your phone
- Add a **button widget** and assign it to GPIO (like D2 or D5)

---