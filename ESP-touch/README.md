# ✨ ESP32 Touch Sensor with LED Trigger

This project demonstrates how to use the **ESP32's built-in capacitive touch sensing** to trigger an LED when a finger touches GPIO pin `T0`.

---

## 📦 Features

- 🔘 Uses ESP32’s **internal capacitive touch sensor** (no extra hardware needed)
- 🔎 Reads touch values using `touchRead(T0)`
- 💡 Turns LED ON when touch value drops below threshold
- 📊 Prints touch readings to Serial Monitor

---

## 🧰 Hardware Required

- ESP32 Dev Module
- 1x LED
- 1x 220Ω resistor
- Jumper wires
- (Optional) Copper foil or coin for a bigger touchpad

---

## 🔌 Wiring

| Component | ESP32 Pin |
|----------|-----------|
| Touch Pad (T0) | GPIO 4 (Touch0) |
| LED          | GPIO 2 |
| Resistor     | In series with LED to GND |

> ⚠️ `touchRead(T0)` corresponds to **GPIO 4**, not 0.  
> But in your code, you're calling `T0` directly — double check which physical pin you're actually touching!

---
