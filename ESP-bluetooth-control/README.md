# 🔵 ESP32 Bluetooth LED Controller

Control an LED via Bluetooth using your phone or any Bluetooth terminal app. This project uses an ESP32 microcontroller and its built-in Bluetooth capabilities.

## 🚀 Features
- Pairs with Bluetooth using the device name: `ESP32-BT-Slave`
- Turns LED **ON** or **OFF** via simple commands (`1` or `0`)
- Clean, minimal, and perfect for beginners 👶 or rapid prototyping 💡

---

## 🧠 Requirements

- ESP32 Dev Board  
- Arduino IDE  
- `BluetoothSerial` library (built-in with ESP32 board support)  
- A Bluetooth terminal app (like [Serial Bluetooth Terminal](https://play.google.com/store/apps/details?id=de.kai_morich.serial_bluetooth_terminal))

---

## ⚙️ Circuit Setup

| ESP32 Pin | Component       |
|-----------|------------------|
| GPIO 2    | LED (w/ 220Ω Resistor) |
| GND       | LED GND          |

> Note: GPIO 2 has an onboard LED in some ESP32 boards. You can use it directly if available.

---

## 📦 How It Works

1. Upload the code via Arduino IDE.
2. Pair your phone with **ESP32-BT-Slave** via Bluetooth.
3. Open a terminal app, connect to the ESP32.
4. Send:
   - `1` to turn **ON** the LED  
   - `0` to turn **OFF** the LED

---
