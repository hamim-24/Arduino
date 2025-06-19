# 🖨️ ESP32 Serial to I2C LCD Writer

Send any text via Serial Monitor and display it instantly on an I2C-connected 16x2 LCD screen. Super handy for IoT dashboards, debugging, or just flexing your serial skills.

---

## 🚀 Features

- Accepts live text from Serial Monitor
- Displays the input on a 16x2 I2C LCD
- Auto-clears old text and shows the latest input
- Beginner-friendly but expandable 💪

---

## 🧠 Requirements

- ESP32 or Arduino board  
- I2C LCD display (16x2)  
- `LiquidCrystal_I2C` library  
- Arduino IDE  
- Jumper wires + breadboard  

---

## 🔌 Wiring

| LCD Pin | Connect To        |
|---------|-------------------|
| VCC     | 3.3V or 5V         |
| GND     | GND               |
| SDA     | ESP32 GPIO 21 (default SDA) |
| SCL     | ESP32 GPIO 22 (default SCL) |

> Use an I2C scanner sketch if you’re unsure of your LCD’s address (commonly `0x27` or `0x3F`).

---

## 📦 How It Works

1. Upload the code.
2. Open **Serial Monitor** (`115200` baud rate).
3. Must change 'New Line' to 'No Line Ending'.
4. Type anything and hit **Enter**.
5. LCD will clear and show your input with a "You Write:" label.

---