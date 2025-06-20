# 🔄 ESP32 Servo Sweep with `ESP32Servo` Library

A basic demo to control a servo motor using **ESP32** and the `ESP32Servo` library. This project makes the servo sweep back and forth between 0° and 180°.

---

## 📦 Features

- 🎯 Sweeps servo from 0° ➡️ 180° and back
- 🔌 Uses GPIO 13 for servo signal
- ⏱️ Smooth delay-based motion
- 🔋 Perfect for robotics, pan-tilt cams, or locks

---

## 🧰 Hardware Requirements

- ESP32 Dev Module
- Servo Motor (e.g. SG90 or MG996R)
- External Power Source (for servo)
- Jumper wires
- Breadboard

---

## ⚠️ Servo Wiring Guide

| Servo Wire | Connect to       |
|------------|------------------|
| Red        | **5V (external)** |
| Brown/Black| GND (shared with ESP32) |
| Yellow/Orange | GPIO 13 (signal) |

> ⚠️ Don't power the servo directly from the ESP32 3.3V or 5V pin — it can reset or burn out your board. Use an external 5V supply with common ground.

---

## 🔧 Code Explanation

```cpp
#include <ESP32Servo.h>

Servo myServo;

void setup() {
  Serial.begin(115200);
  myServo.attach(13); // Connects servo signal to GPIO 13
}
```
## 📚 Library Installation

Install via Arduino Library Manager:
-	ESP32Servo by Kevin Harrington, John K. Bennett