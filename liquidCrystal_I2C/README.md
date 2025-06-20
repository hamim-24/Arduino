# ❤️ Arduino + I2C LCD with Custom Heart Character

This project uses an **Arduino Uno/Nano** with a **16x2 I2C LCD** to display a custom heart icon and the word `"Hello"` — all over just two I2C pins. Minimal wiring, max style 💬💖

---

## 📦 Features

- 💡 I2C LCD display with minimal pin use
- ❤️ Custom heart character via `lcd.createChar()`
- ✅ Compatible with Arduino Uno, Nano, Mega, etc.
- 📟 Static display: `[♥] Hello`

---

## 🧰 Hardware Required

- Arduino Uno / Nano / Mega
- I2C LCD 16x2 (usually address `0x27` or `0x3F`)
- Jumper wires

---

## 🔌 Arduino I2C Pin Mapping

| Arduino Pin | Function |
|-------------|----------|
| A4          | SDA      |
| A5          | SCL      |

✅ These are standard across Uno and Nano boards. If you're using Mega, the I2C pins are `SDA = 20`, `SCL = 21`.

---

## ⚙️ Wiring

| LCD Pin | Arduino Pin |
|---------|-------------|
| GND     | GND         |
| VCC     | 5V          |
| SDA     | A4          |
| SCL     | A5          |

---
## 📚 Libraries Required

Install via Arduino Library Manager:
-	LiquidCrystal_I2C
-	Wire.h (built-in)
## 🧪 Troubleshooting

-	If nothing shows up:
-	Use I2C Scanner to find the LCD’s correct address
-	Try 0x3F if 0x27 doesn’t work
-	Adjust contrast using potentiometer on LCD module
-	Confirm SDA/SCL wiring (A4 and A5)