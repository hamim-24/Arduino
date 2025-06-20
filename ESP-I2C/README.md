# 📟 ESP32 with I2C LCD Display (16x2)

A simple project using an **ESP32** and an **I2C-based 16x2 LCD display** to print static messages. Perfect for IoT dashboards, sensors, or embedded UI feedback.

---

## 📦 Features

- 🧠 Uses `LiquidCrystal_I2C` to drive I2C LCDs
- ✅ Tested with I2C address `0x27`
- 📟 16 columns × 2 rows display
- 💡 Prints a welcome message using custom ESP32 I2C pins

---

## 🧰 Hardware Required

- ESP32 Dev Board
- I2C 16x2 LCD display (typically address `0x27` or `0x3F`)
- Jumper wires
- Breadboard

---

## 🔌 Wiring Diagram

| LCD Pin | ESP32 Pin |
|---------|-----------|
| GND     | GND       |
| VCC     | 3.3V or 5V |
| SDA     | GPIO 21   |
| SCL     | GPIO 22   |

> 📍 These are default I2C pins on most ESP32 boards:
> - **SDA = GPIO 21**
> - **SCL = GPIO 22**

---

## 💻 Code Overview

### ✅ `setup()`
- Initializes I2C with custom pins
- Turns on LCD backlight
- Prints two lines of text:

### 🌀 `loop()`
- Currently empty — customize it to show sensor data, time, etc.

---

## 📦 Libraries Required

Install via Arduino Library Manager:

- [`LiquidCrystal_I2C`](https://github.com/johnrickman/LiquidCrystal_I2C)
- `Wire.h` (built-in)

---

## 🛠️ Setup Instructions

1. Open **Arduino IDE**
2. Select board: **ESP32 Dev Module**
3. Paste the code into a new sketch
4. Upload and watch the LCD display come alive 🔥

---

## 🔍 I2C Address Troubleshooting

If your display doesn’t work:
- Run an **I2C scanner sketch** to find the correct address
- Common addresses: `0x27`, `0x3F`

> [📜 I2C Scanner Sketch](https://playground.arduino.cc/Main/I2cScanner/)

---

## 📸 Preview

> Add a photo or short video of your LCD screen in action

---
