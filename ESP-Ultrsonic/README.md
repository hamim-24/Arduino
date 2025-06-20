# 📏 ESP32 + HC-SR04 Ultrasonic Distance Sensor

This project uses the **HC-SR04 ultrasonic sensor** with an **ESP32** to measure distance in centimeters and output the results to the Serial Monitor.

---

## 📦 Features

- 🧠 Measures distance using sound waves (echolocation style)
- 📤 Sends pulses with `TRIG_PIN`
- 📥 Reads time for echo return on `ECHO_PIN`
- 🔎 Converts echo duration to distance (cm)
- 📈 Displays readings every 500ms via Serial

---

## 🧰 Hardware Requirements

- ESP32 Dev Board
- HC-SR04 Ultrasonic Distance Sensor
- Jumper wires
- Breadboard (optional)

---

## ⚙️ Pin Configuration

| HC-SR04 Pin | ESP32 Pin |
|-------------|-----------|
| VCC         | 5V        |
| GND         | GND       |
| TRIG        | GPIO 14   |
| ECHO        | GPIO 27   |

> ⚠️ ESP32 operates at 3.3V logic, but HC-SR04 is generally 5V-tolerant. If you're cautious, use a **voltage divider** on ECHO pin.

---

## 💡 How It Works

1. `TRIG` sends a 10µs HIGH pulse
2. Sensor emits ultrasonic wave
3. Wave bounces off an object and returns
4. `ECHO` pin stays HIGH for the duration the wave took to return
5. Code calculates distance using:
` distance = duration * 0.0343 / 2; `