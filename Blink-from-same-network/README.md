# 🌐 ESP32 LED Controller via Web Server (SoftAP Mode)

This project allows you to control two LEDs using your **ESP32 board** as a **Wi-Fi access point** and host a **web interface** directly—no internet required.

---

## 📦 Features

- ✅ ESP32 acts as its own Wi-Fi network (SoftAP mode)
- ✅ Host an HTML web page with control buttons
- ✅ Turn ON/OFF LEDs on GPIO 15 and GPIO 2
- ✅ Fully offline — perfect for remote/local control
- ✅ Simple UI, mobile and desktop friendly

---

## 🧰 Hardware Requirements

- ESP32 Dev Module or ESP32-CAM
- 2x LEDs
- 2x 220Ω resistors
- Breadboard & jumper wires
- USB cable for programming

---

## 📐 Circuit Diagram

| GPIO Pin | Component |
|----------|-----------|
| GPIO 15  | LED 1     |
| GPIO 2   | LED 2     |
| GND      | GND of both LEDs (through resistors) |

💡 Add resistors in series with the LEDs to prevent overcurrent.

---

## 📶 Wi-Fi Access Point Details

| Setting   | Value             |
|-----------|------------------|
| SSID      | ESP32-Network     |
| Password  | Esp32-Password    |
| IP        | `192.168.4.1`     (default SoftAP IP)

---

## 🧠 How It Works

- ESP32 starts in **SoftAP** mode (Access Point).
- It runs a web server on port `80`.
- Client connects to Wi-Fi and navigates to (Ex: `192.168.4.1`).
- Web page displays two LED toggle buttons.
- HTTP GET requests control the GPIOs.

---

## 🧪 How to Use

1. **Upload the code** to your ESP32 using Arduino IDE.
2. Connect to Wi-Fi:
   - SSID: `ESP32-Network`
   - Password: `Esp32-Password`
3. Open a browser and go to: 
    ```
        http://192.168.4.1
    ```
    4. Click buttons to toggle LEDs ON/OFF.

---

## 🖥️ Web Interface Example

- Button text and color changes based on current LED state.
- All updates reflect in real-time with HTTP requests.

| URL             | Action        |
|-----------------|---------------|
| `/16/on`        | Turn ON LED1  |
| `/16/off`       | Turn OFF LED1 |
| `/17/on`        | Turn ON LED2  |
| `/17/off`       | Turn OFF LED2 |

---

## 🧱 Libraries Used

- [`WiFi.h`](https://www.arduino.cc/en/Reference/WiFi): Built-in with ESP32 core

No external libraries are required.

---

## ⚠️ Notes

- Don’t use GPIO1/3 for LEDs — those are UART serial pins.
- GPIO2 is connected to on-board LED on some ESP32s — behavior may vary.
- Ensure your browser doesn’t auto-cache the page — refresh if needed.

---
