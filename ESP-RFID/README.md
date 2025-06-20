# 📡 ESP32 + MFRC522 RFID Reader

This project demonstrates how to interface the **MFRC522 RFID module** with an **ESP32** using the SPI protocol to read and display RFID card UIDs over Serial.

---

## 📦 Features

- 🔐 Reads RFID/NFC tags (13.56 MHz)
- 🚀 Uses hardware SPI on ESP32 for fast reads
- 🖨️ Prints UID to Serial Monitor in HEX
- 🧠 Includes utility to format UID as readable string

---

## 🧰 Hardware Requirements

- ESP32 Dev Module
- MFRC522 RFID Reader (RC522 module)
- RFID Tags/Cards
- Jumper Wires

---

## 🔌 Wiring Diagram (ESP32 ↔ RC522)

| RC522 Pin | ESP32 Pin |
|-----------|-----------|
| VCC       | **3.3V**  |
| GND       | GND       |
| RST       | GPIO 4    |
| SDA (SS)  | GPIO 5    |
| SCK       | GPIO 18   |
| MOSI      | GPIO 23   |
| MISO      | GPIO 19   |

> ⚠️ **Use 3.3V for VCC** — RC522 is **not 5V tolerant!**

---

## 💻 Code Overview

### `setup()`
- Initializes Serial and SPI
- Starts the MFRC522 module
- Prompts for card scanning

### `loop()`
- Checks for new card
- Reads UID
- Formats and prints UID

### `getUIDString()`
- Converts byte UID array to uppercase hex string
- Adds leading zeros for clean formatting

---

## 📜 Sample Output
```
    Scan RFID Card
    UID: 04 D3 5A 6F
```
---

## 🛠️ Libraries Required

Install via Arduino Library Manager:

- [`MFRC522`](https://github.com/miguelbalboa/rfid) by Miguel Balboa
- `SPI` (built-in with ESP32 core)

---

## 🧪 Tips & Troubleshooting

- ✅ Ensure you power with **3.3V**, not 5V
- ✅ Always call `SPI.begin()` before `PCD_Init()`
- ✅ Use **Serial Monitor @ 115200 baud**
- ✅ Cards should be **13.56 MHz ISO/IEC 14443 A**

---
