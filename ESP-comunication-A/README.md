# 📡 ESP32 Chat over Wi-Fi

This project is a simple **two-way chat system between two ESP32 devices** over Wi-Fi — like a mini Messenger or WhatsApp, but running on microcontrollers. [Another file](https://github.com/hamim-24/Arduino/tree/main/ESP-comunication-B)

---

## 🚀 Features

- 🔁 Real-time two-way messaging
- 📡 Peer-to-peer communication over local Wi-Fi
- 💬 Send and receive messages via Serial Monitor
- 🪶 Lightweight and minimal

---

## 🛠 Requirements

- 2x ESP32 Development Boards
- Arduino IDE (or PlatformIO)
- A common 2.4GHz Wi-Fi network
- USB cables for flashing & Serial communication

---

## ⚙️ Setup Instructions

### 1️⃣ Flash Code to Both ESP32s

Use the same code on both devices. Change these values on **each ESP**:

```cpp
// For Device A:
const char* peerIP = ""; //Put IP of Device B
String myName = "ESP32-A";

// For Device B:
const char* peerIP = ""; //Put IP of Device A
String myName = "ESP32-B";

```

## Connect Your Wifi
```cpp
String ssid = "";
String password = "";
```

## Troubleshooting 
|Problem|Fix|
|---------------------------|-----------------------------------------------|
|“Failed to send message”|Check if peer IP is correct & peer is online|
|No messages received|Make sure both ESPs have correct IPs & server started|
|Gibberish on Serial Monitor|Set baud to 115200|
|Both ESPs have same IP|Restart router or devices|


