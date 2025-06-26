# ESP32 WiFi Chat with LCD Display

This project demonstrates a simple two-way chat system between two ESP32 development boards using WiFi. Messages are sent and received over a local network, displayed on an I2C LCD, and indicated by LED lights.

## Features

*   **Two-Way Communication:** Send and receive messages between two ESP32 devices.
*   **WiFi Connectivity:** Utilizes ESP32's built-in WiFi for network communication.
*   **I2C LCD Display:** Shows incoming and outgoing messages on a 16x2 character LCD.
*   **LED Indicators:** Visual feedback for sent (red LED) and received (green LED) messages.
*   **Serial Monitor Interface:** Send messages from and view messages on the Arduino IDE's Serial Monitor.

## Hardware Requirements

*   2 x ESP32 Development Boards
*   2 x I2C 16x2 LCD Displays (e.g., with PCF8574 adapter)
*   2 x Red LEDs
*   2 x Green LEDs
*   2 x 220 Ohm Resistors (for LEDs, optional but recommended)
*   Jumper Wires
*   Breadboards (optional)

## Pin Configuration

This project uses the following pin assignments on the ESP32:

*   **Red LED:** GPIO 5
*   **Green LED:** GPIO 4
*   **I2C LCD:**
    *   **SDA (Data Line):** GPIO 21
    *   **SCL (Clock Line):** GPIO 22

**Wiring Diagram (for one ESP32):**

| Component      | ESP32 Pin |
| :------------- | :-------- |
| Red LED (+)    | GPIO 5    |
| Red LED (-)    | GND (via resistor) |
| Green LED (+)  | GPIO 4    |
| Green LED (-)  | GND (via resistor) |
| LCD SDA        | GPIO 21   |
| LCD SCL        | GPIO 22   |
| LCD VCC        | 3.3V / 5V |
| LCD GND        | GND       |

## Software Requirements

*   Arduino IDE
*   ESP32 Board Package for Arduino IDE
*   `LiquidCrystal_I2C` Library (Install via Arduino IDE Library Manager)

## Setup Instructions

1.  **Install Libraries:**
    *   Open Arduino IDE.
    *   Go to `Sketch > Include Library > Manage Libraries...`
    *   Search for `LiquidCrystal_I2C` and install the library by `Frank de Brabander`.

2.  **Configure WiFi Credentials:**
    *   In the provided Arduino sketch, update the `ssid` and `password` variables with your local WiFi network's credentials:
    ```cpp
    String ssid = "YOUR_WIFI_SSID";
    String password = "YOUR_WIFI_PASSWORD";
    ```

3.  **Assign Device Names and Peer IPs:**
    *   You will need two ESP32 devices. For each device, modify the `myName` and `peerIP` variables:

    **For ESP32 Device A:**
    ```cpp
    String myName = "A";
    const char* peerIP = "192.168.0.104"; // <-- Set this to the expected IP of ESP32 Device B
    ```

    **For ESP32 Device B:**
    ```cpp
    String myName = "B";
    const char* peerIP = "192.168.0.103"; // <-- Set this to the expected IP of ESP32 Device A
    ```
    **Important:** You need to know the IP addresses your router assigns to your ESP32s. You can find this by uploading the code to one ESP32, opening the Serial Monitor, and noting the "Connected. IP:" address. Then, update the `peerIP` on the other device accordingly.

4.  **Upload Code:**
    *   Select the correct ESP32 board and COM port in the Arduino IDE.
    *   Upload the sketch to both ESP32 devices.

## Usage

1.  **Power On:** Connect both ESP32 devices to power.
2.  **Connect to WiFi:** Both devices will attempt to connect to the specified WiFi network. Their assigned IP addresses will be printed to the Serial Monitor.
3.  **Open Serial Monitor:** For each ESP32, open its respective Serial Monitor in the Arduino IDE (set baud rate to 115200).
4.  **Start Chatting:**
    *   Type a message in the input bar of one ESP32's Serial Monitor and press Enter.
    *   The message will be sent to the other ESP32, displayed on its LCD, and printed to its Serial Monitor.
    *   The red LED will blink on the sending device, and the green LED will blink on the receiving device.
    *   You can reply from the other ESP32's Serial Monitor.

## Potential Improvements

*   **Dynamic IP Discovery (mDNS):** Instead of hardcoding `peerIP`, implement mDNS to allow devices to discover each other by name (e.g., `esp32-chat-a.local`).
*   **WiFi Credential Management:** Use a library like `WiFiManager` to allow users to configure WiFi credentials via a captive portal, making the setup more user-friendly.
*   **LCD Flicker Reduction:** Optimize LCD updates to avoid `lcd.clear()` on every message, which causes flickering. Only update the specific lines or characters that have changed.
*   **Robust Communication:** For more complex or multi-device scenarios, consider using a messaging protocol like MQTT with a central broker.
*   **Error Handling:** Implement more robust error handling for network disconnections or failed message transmissions.
*   **Message History:** Store a short history of messages to display more than just the last incoming/outgoing message.
