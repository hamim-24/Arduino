# 💡 ESP32 Serial LED Controller

Control an LED using text commands sent over the Serial Monitor (USB). This is a beginner-friendly project for anyone diving into ESP32, IoT, or embedded systems.

---

## 🚀 Features

- Control LED via USB Serial commands  
- Accepts `"on"` and `"off"` string commands  
- Easy to integrate into bigger projects like IoT dashboards or CLI interfaces  

---

## 🧠 Requirements

- ESP32 Dev Board  
- Arduino IDE  
- USB Cable  
- LED (or use onboard LED on GPIO 2)  

---

## 🔌 Wiring

| ESP32 Pin | Component         |
|-----------|-------------------|
| GPIO 2    | LED (+ Resistor)  |
| GND       | LED GND           |

> You can skip the external LED if your board has a built-in LED on GPIO 2.

---

## 📦 How It Works

1. Upload the code to your ESP32 using Arduino IDE.
2. Open **Serial Monitor** (baud: `115200`).
3. Must change 'New Line' to 'No Line Ending'.
4. Type:
   - `"on"` → LED turns ON 🔆  
   - `"off"` → LED turns OFF 💤  
5. You’ll also get feedback printed in Serial Monitor.

---