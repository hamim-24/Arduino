# Arduino RFID & Ultrasonic Security Gate

This project implements a simple security gate system using an Arduino, an RFID reader, an ultrasonic sensor, servo motors, an LCD display, and visual/auditory indicators. The system detects objects, verifies RFID cards, and requires a password for access, controlling a gate servo accordingly.

## Features

*   **Object Detection:** Utilizes an ultrasonic sensor to detect the presence of an object (e.g., a person or vehicle) within a specified range.
*   **RFID Authentication:** Reads RFID tags/cards and compares them against a predefined allowed UID for access verification.
*   **Password Verification:** After successful RFID authentication, a password must be entered via the serial monitor for full access.
*   **Automated Gate Control:** A servo motor acts as a gate, opening upon successful authentication and closing automatically after a delay.
*   **Visual Feedback:** Red and green LEDs indicate system status (object detected, access granted/denied).
*   **Auditory Feedback:** A buzzer provides sound alerts for various events (e.g., gate closing countdown, unauthorized access, wrong password).
*   **LCD Display:** A 16x2 I2C LCD provides user prompts and status messages.
*   **Continuous Scanning:** When no object is detected, a servo continuously sweeps, indicating the system is active and scanning the area.

## Components Required

To build this project, you will need the following hardware components:

*   Arduino Uno (or compatible board)
*   MFRC522 RFID Reader Module
*   RFID Tags/Cards (compatible with MFRC522)
*   HC-SR04 Ultrasonic Sensor
*   2x SG90 Servo Motors (or similar small servos)
*   16x2 I2C LCD Display Module
*   1x Red LED
*   1x Green LED
*   1x Passive Buzzer
*   Breadboard
*   Jumper Wires

## Wiring Instructions

Connect the components to your Arduino board as follows:

| Component                 | Component Pin | Arduino Pin   | Notes                                                              |
| :------------------------ | :------------ | :------------ | :----------------------------------------------------------------- |
| **MFRC522 RFID Reader**   | SDA (SS)      | Digital 10    |                                                                    |
|                           | RST           | Digital 9     |                                                                    |
|                           | MOSI          | Digital 11    |                                                                    |
|                           | MISO          | Digital 12    |                                                                    |
|                           | SCK           | Digital 13    |                                                                    |
|                           | VCC           | 3.3V          |                                                                    |
|                           | GND           | GND           |                                                                    |
| **HC-SR04 Ultrasonic Sensor** | Trig          | Digital 8     |                                                                    |
|                           | Echo          | Digital 7     |                                                                    |
|                           | VCC           | 5V            |                                                                    |
|                           | GND           | GND           |                                                                    |
| **Servo Motor 1 (Sweeping)** | Signal        | Digital 6     |                                                                    |
|                           | VCC           | 5V            | Or external power supply if using multiple servos                  |
|                           | GND           | GND           |                                                                    |
| **Servo Motor 2 (Gate)**  | Signal        | Digital 3     |                                                                    |
|                           | VCC           | 5V            | Or external power supply if using multiple servos                  |
|                           | GND           | GND           |                                                                    |
| **16x2 I2C LCD Display**  | SDA           | Analog A4     | (or dedicated SDA pin on your Arduino)                             |
|                           | SCL           | Analog A5     | (or dedicated SCL pin on your Arduino)                             |
|                           | VCC           | 5V            |                                                                    |
|                           | GND           | GND           |                                                                    |
| **Red LED**               | Anode (+)     | Digital 5     | Connect via a current-limiting resistor (e.g., 220 Ohm)            |
|                           | Cathode (-)   | GND           |                                                                    |
| **Green LED**             | Anode (+)     | Digital 4     | Connect via a current-limiting resistor (e.g., 220 Ohm)            |
|                           | Cathode (-)   | GND           |                                                                    |
| **Passive Buzzer**        | Signal        | Digital 2     |                                                                    |
|                           | GND           | GND           |                                                                    |

## Software Setup

1.  **Arduino IDE:** Download and install the [Arduino IDE](https://www.arduino.cc/en/software).
2.  **Install Libraries:**
    *   Open the Arduino IDE.
    *   Go to `Sketch` > `Include Library` > `Manage Libraries...`
    *   Search for and install the following libraries:
        *   `MFRC522` by UIPEthernet
        *   `Servo` (usually pre-installed)
        *   `LiquidCrystal I2C` by Frank de Brabander
    *   Ensure `Wire.h` and `SPI.h` are available (these are standard Arduino libraries).

## How to Use

1.  **Upload Code:** Open the provided `.ino` file in the Arduino IDE and upload it to your Arduino board.
2.  **Power On:** Connect your Arduino to a power source.
3.  **Object Detection:** The system will start with the sweeping servo. When an object comes within 20cm of the ultrasonic sensor, the red LED will turn on, and the LCD will display "Who are you? Scan for Verify".
4.  **RFID Scan:** Present an allowed RFID card to the MFRC522 reader.
5.  **Password Entry:** If the card is valid, the LCD will prompt "Enter password : ". Open the Serial Monitor in the Arduino IDE (Tools > Serial Monitor) and type the password (`1234` by default), then press Enter.
6.  **Access Granted/Denied:**
    *   If the password is correct, the green LED will turn on, the gate servo will open, and a countdown will be displayed on the LCD before the gate closes automatically.
    *   If the password is incorrect or the RFID card is unauthorized, the red LED will turn on, and an error message will be displayed on the LCD with a buzzer sound.
7.  **System Reset:** After an access attempt (successful or not), the system will return to the object detection state, and the sweeping servo will resume.

## Customization

You can easily customize the following parameters in the code:

*   **Allowed RFID Card UID:**
    *   To find the UID of your RFID card, uncomment the `showUID()` function call in `loop()` temporarily, upload the code, and scan your card. The UID will be printed to the Serial Monitor.
    *   Update the `allowedCard` variable in the code with your card's UID:
        \`\`\`cpp
        MFRC522::Uid allowedCard = { 4, { 0xXX, 0xXX, 0xXX, 0xXX }, 0 }; // Replace XX with your UID bytes
        \`\`\`
*   **Password:**
    *   Change the `correctPwd` variable to your desired password:
        \`\`\`cpp
        String correctPwd = "your_new_password";
        \`\`\`
*   **Ultrasonic Detection Distance:**
    *   Modify the `20` in `if (measureDistance() < 20)` within the `detectObject()` function to change the detection threshold in centimeters.
*   **Servo Positions:**
    *   Adjust `servo.write(90)` and `servo1.write(70)` for initial positions.
    *   Adjust `servo1.write(0)` for the open gate position.
*   **Gate Open Duration:**
    *   Change the loop `for (int i = 0; i < 6; i++)` to modify how long the gate stays open (6 iterations * 1 second delay = 6 seconds).
