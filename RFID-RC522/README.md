# 🔐 Arduino RFID + Password Authentication System

This project combines **RFID card scanning** with **manual password entry** to create a **2-Factor Authentication system** using an Arduino and MFRC522 module. It’s like a mini bank vault for your projects 🏦✨

---

## 📦 Features

- 🪪 RFID card detection via MFRC522
- 🔢 Manual password input through Serial Monitor
- ✅ Access granted only when **card + password** match
- 🚨 LED indicator for success
- 🔒 Denies access for unauthorized card or wrong password

---

## 🧰 Hardware Required

- Arduino Uno / Nano / Mega
- MFRC522 RFID Reader
- RFID Card/Tag (13.56 MHz)
- 1x LED
- 1x 220Ω resistor
- Jumper wires

---

## ⚙️ Pin Connections

| MFRC522 Pin | Arduino Pin |
|-------------|-------------|
| SDA         | D10         |
| SCK         | D13         |
| MOSI        | D11         |
| MISO        | D12         |
| RST         | D9          |
| VCC         | 3.3V        |
| GND         | GND         |

> ⚠️ **Use 3.3V**, not 5V — RC522 is not 5V-tolerant!

---

## 💡 LED Connection

- LED ➡️ 220Ω Resistor ➡️ D8  
- Common GND

---

## 🔐 Access Flow

1. User scans RFID card
2. If card UID matches `allowedCard`, Arduino asks for password
3. User types password via Serial Monitor
4. If password matches, LED turns ON (simulating door unlock)
5. Otherwise, access is denied

---

## 🔎 Code Highlights

### ✅ Set Allowed UID

```cpp
MFRC522::Uid allowedCard = { 4, { 0x16, 0xDF, 0x57, 0xF9 }, 0 };
```
>Replace 0x16, 0xDF, 0x57, 0xF9 with your card’s actual UID (watch Serial output)

## 🔐 Password Check
```cpp
String correctPassword = "1234";
```
## 🧪 How to Get Your Card UID
```cpp
showUID();  // Shows UID of any scanned card
```