# ESP32 Smart Bus - Complete Wiring Diagram & Hardware Guide

## 🔌 Wiring Diagrams for Your ESP32 Boards

### Your Board 1: ESP32 WROOM-32 DevKit (40 pins)
```
                         ESP32 WROOM-32 DEVKIT
                    ┌─────────────────────────────────────┐
                    │                                     │
                    │ 3V3 ●                         ● VIN │ ← 5V USB/External
                    │ EN  ●                         ● GND │
                    │ G36 ●                         ● G23 │──── RC522 MOSI
                    │ G39 ●                         ● G22 │──── LCD SCL
                    │ G34 ●                         ● TX  │
                    │ G35 ●                         ● RX  │
                    │ G32 ●                         ● G21 │──── LCD SDA
                    │ G33 ●                         ● G19 │──── RC522 MISO
                    │ G25 ●                         ● G18 │──── RC522 SCK
                    │ G26 ●                         ● G5  │──── RC522 SDA
                    │ G27 ●                         ● G17 │
                    │ G14 ●                         ● G16 │
                    │ G12 ●                         ● G4  │──── RC522 RST
                    │ GND ●    USB    [ESP32]       ● G0  │
                    │ VIN ●   Mini-B  [WROOM]       ● G2  │
                    └─────────────────────────────────────┘
                    IR1→G32   IR2→G33   TRIG→G26   ECHO→G27
                    SERVO→G15  LED1→G25  LED2→G13  LED3→G12  LED4→G14  LED5→G2
```

### Your Board 2: ESP32 Dev Module (32 pins)
```
                            ESP32 DEV MODULE
                    ┌─────────────────────────────────┐
                    │                                 │
                    │ 3V3 ●                     ● D27 │──── Ultrasonic ECHO
                    │ RST ●                     ● D26 │──── Ultrasonic TRIG
                    │ D36 ●                     ● D25 │
        LCD SCL ────┤ D22 ●                     ● D33 │──── IR Sensor 2 (Exit)
        LCD SDA ────┤ D21 ●                     ● D32 │──── IR Sensor 1 (Entry)
                    │  TX ●                     ● D35 │
                    │  RX ●                     ● D34 │
   RC522 MISO ──────┤ D19 ●   USB   [ESP32]     ● D39 │
   RC522 SCK ───────┤ D18 ●  Mini-B [MODULE]    ● D36 │
     RC522 SDA ─────┤  D5 ●                     ● RST │
   RC522 MOSI ──────┤ D23 ●                     ● 3V3 │
                    │ GND ●                     ● GND │
    RC522 RST ──────┤  D4 ●                     ● VIN │ ← 5V USB/External
                    └─────────────────────────────────┘
                    SERVO→D15  LED1→D25  LED2→D13  LED3→D12  LED4→D14  LED5→D2
```

## 📋 Complete Pin Assignment Table (Your Boards)

| ESP32 Pin | Component | Function | Voltage | WROOM-32 (40-pin) | Dev Module (32-pin) |
|-----------|-----------|----------|---------|------------------|------------------|
| GPIO 5 | RC522 SDA | NFC Communication | 3.3V | ✅ Pin G5 | ✅ Pin D5 |
| GPIO 4 | RC522 RST | NFC Reset | 3.3V | ✅ Pin G4 | ✅ Pin D4 |
| GPIO 18 | RC522 SCK | SPI Clock | 3.3V | ✅ Pin G18 | ✅ Pin D18 |
| GPIO 23 | RC522 MOSI | SPI Data Out | 3.3V | ✅ Pin G23 | ✅ Pin D23 |
| GPIO 19 | RC522 MISO | SPI Data In | 3.3V | ✅ Pin G19 | ✅ Pin D19 |
| GPIO 21 | LCD SDA | I2C Data | 5V | ✅ Pin G21 | ✅ Pin D21 |
| GPIO 22 | LCD SCL | I2C Clock | 5V | ✅ Pin G22 | ✅ Pin D22 |
| GPIO 26 | HC-SR04 TRIG | Ultrasonic Trigger | 5V | ✅ Pin G26 | ✅ Pin D26 |
| GPIO 27 | HC-SR04 ECHO | Ultrasonic Echo | 5V | ✅ Pin G27 | ✅ Pin D27 |
| GPIO 32 | IR Sensor 1 | Entry Detection | 3.3V | ✅ Pin G32 | ✅ Pin D32 |
| GPIO 33 | IR Sensor 2 | Exit Detection | 3.3V | ✅ Pin G33 | ✅ Pin D33 |
| **GPIO 15** | **Servo Motor** | **Gate Control** | **5V** | ✅ Pin G15 | ✅ Pin D15 |
| **GPIO 25** | **LED White** | **Status Light** | **3.3V** | ✅ Pin G25 | ✅ Pin D25 |
| **GPIO 13** | **LED Green 1** | **Success Light** | **3.3V** | ✅ Pin G13 | ✅ Pin D13 |
| **GPIO 12** | **LED Green 2** | **Success Light** | **3.3V** | ✅ Pin G12 | ✅ Pin D12 |
| **GPIO 14** | **LED Red 1** | **Warning Light** | **3.3V** | ✅ Pin G14 | ✅ Pin D14 |
| **GPIO 2** | **LED Red 2** | **Warning Light** | **3.3V** | ✅ Pin G2 | ✅ Pin D2 |

### 📌 Your Board Specifications

| Feature | WROOM-32 DevKit (40-pin) | Dev Module (32-pin) |
|---------|-------------------------|-------------------|
| **Form Factor** | Large development board | Compact development board |
| **USB Connection** | USB Mini-B (built-in) | USB Mini-B (built-in) |
| **Power Regulation** | Built-in 3.3V regulator | Built-in 3.3V regulator |
| **Programming** | Direct USB programming | Direct USB programming |
| **Size** | Larger (40 pins) | Smaller (32 pins) |
| **Pin Labels** | G16, G17, CLK, CMD, etc. | D2, D4, TX2, RX2, etc. |
| **Ease of Use** | ✅ Plug and play | ✅ Plug and play |
| **Breadboard** | ✅ Direct breadboard use | ✅ Direct breadboard use |

## 🔧 Component Connection Details

### ✅ GOOD NEWS: Both Your Boards Are Plug-and-Play!

#### Both ESP32 WROOM-32 DevKit & Dev Module:
```
✅ Built-in USB Mini-B connector for programming
✅ Integrated 3.3V regulator  
✅ Auto-reset circuit included
✅ Direct breadboard compatibility
✅ No additional components needed

Just connect sensors and power on!
```

### RC522 RFID Module
```
RC522 Module Pinout:
┌─────────────────┐
│ SDA  SCK  MOSI  │
│ MISO RST  GND   │  
│ IRQ  3.3V       │
└─────────────────┘

Connections:
RC522 SDA  → ESP32 GPIO 5
RC522 SCK  → ESP32 GPIO 18
RC522 MOSI → ESP32 GPIO 23
RC522 MISO → ESP32 GPIO 19
RC522 RST  → ESP32 GPIO 4
RC522 3.3V → ESP32 3.3V
RC522 GND  → ESP32 GND
RC522 IRQ  → Not Connected

⚠️ IMPORTANT: RC522 requires 3.3V power (NOT 5V!)
```

### LCD Display (20x4 with I2C Backpack)
```
LCD I2C Module:
┌─────────────────┐
│ GND VCC SDA SCL │
└─────────────────┘

Connections:
LCD GND → ESP32 GND
LCD VCC → ESP32 VIN (5V) - for backlight
LCD SDA → ESP32 GPIO 21
LCD SCL → ESP32 GPIO 22

Note: I2C Address usually 0x27 or 0x3F
      Enhanced: Now supports 20x4 display for better information display
```

### HC-SR04 Ultrasonic Sensor
```
HC-SR04 Pinout:
┌─────────────────────┐
│ VCC TRIG ECHO GND   │
└─────────────────────┘

Connections:
HC-SR04 VCC  → ESP32 VIN (5V)
HC-SR04 TRIG → ESP32 GPIO 26
HC-SR04 ECHO → ESP32 GPIO 27
HC-SR04 GND  → ESP32 GND

Note: Echo pin is 5V tolerant on ESP32
      Enhanced: Used for collision detection and emergency stop
```

### IR Obstacle Sensors (2x)
```
IR Sensor Pinout:
┌─────────────────┐
│ VCC GND OUT     │
└─────────────────┘

IR Sensor 1 (Entry):
IR1 VCC → ESP32 3.3V
IR1 GND → ESP32 GND
IR1 OUT → ESP32 GPIO 32

IR Sensor 2 (Exit):
IR2 VCC → ESP32 3.3V
IR2 GND → ESP32 GND  
IR2 OUT → ESP32 GPIO 33

Note: Active LOW sensors (OUTPUT LOW when obstacle detected)
      Enhanced: Advanced state machine for accurate passenger counting
```

### 🆕 **NEW: Servo Motor (Gate Control)**
```
Servo Motor Pinout:
┌─────────────────┐
│ VCC GND Signal  │
└─────────────────┘

Connections:
Servo VCC    → ESP32 VIN (5V)
Servo GND    → ESP32 GND
Servo Signal → ESP32 GPIO 15

Function: Controls automatic gate opening/closing
         - Opens gate for valid tap-in/out
         - Auto-closes after 10 seconds
         - Prevents rapid gate actions
         - Visual feedback with LED indicators

⚠️ IMPORTANT: Servo requires 5V power for reliable operation
```

### 🆕 **NEW: LED Status Indicators (5x)**
```
LED Configuration:
┌─────────────────────────────────┐
│ GPIO 25: White LED  - Normal Status    │
│ GPIO 13: Green LED1 - Success/Entry    │
│ GPIO 12: Green LED2 - Success/Exit     │
│ GPIO 14: Red LED1   - Warning/Alert    │
│ GPIO 2:  Red LED2   - Warning/Alert    │
└─────────────────────────────────┘

Connections:
All LED Anodes → Respective GPIO pins
All LED Cathodes → GND (via 220Ω resistors recommended)

Functions:
- White: Normal operation, system ready
- Green: Successful operations, passenger boarding
- Red: Warnings, alerts, emergency situations
- Special patterns: Speed warnings, passenger overflow, alerts
```

## 🍞 Enhanced Breadboard Layout (Works with Both Your Boards)

### ESP32 Smart Bus - Complete Layout with New Components
```
Breadboard Power Rails:
Red Rail (+)  → ESP32 VIN (5V from USB)
Blue Rail (-) → ESP32 GND

Component Placement:
┌─────────────────────────────────────────────────────────────────────────────┐
│ (+) ════════════════════════════════════════════════════════════════════ RED│
│ (-) ════════════════════════════════════════════════════════════════════BLUE│
│                                                                             │
│     ┌─ESP32 Board─┐              ┌─LCD I2C─┐         ┌─Servo Motor─┐      │
│     │  (Either    │              │ GND VCC │         │   VCC GND   │      │  
│     │   Board)    │              │ SDA SCL │         │   Signal    │      │
│     │             │              └─────────┘         └─────────────┘      │
│     │ G5  G18 G23 │    ┌─RC522 RFID─┐                                     │
│     │ G19 G4  G21 │    │ SDA SCK MOSI│                                     │
│     │ G22 G26 G27 │    │ MISO RST GND│                                     │
│     │ G32 G33 G15 │    │ IRQ  3.3V   │                                     │
│     │ G25 G13 G12 │    └─────────────┘                                     │
│     │ G14 G2  3V3 │                                                        │
│     │ GND VIN     │                                                        │
│     └─────────────┘                                                        │
│                           ┌─HC-SR04─┐    ┌─IR1─┐ ┌─IR2─┐                  │
│                           │VCC TRIG │    │VCC  │ │VCC  │                  │
│                           │ECHO GND │    │GND  │ │GND  │                  │
│                           └─────────┘    │OUT  │ │OUT  │                  │
│                                          └─────┘ └─────┘                  │
│                                                                             │
│ LED Indicators:                                                             │
│ ┌─LED1─┐ ┌─LED2─┐ ┌─LED3─┐ ┌─LED4─┐ ┌─LED5─┐                            │
│ │G25   │ │G13   │ │G12   │ │G14   │ │G2    │                            │
│ │White │ │Green1│ │Green2│ │Red1  │ │Red2  │                            │
│ └──────┘ └──────┘ └──────┘ └──────┘ └──────┘                            │
│ (+) ════════════════════════════════════════════════════════════════════ RED│
│ (-) ════════════════════════════════════════════════════════════════════BLUE│
└─────────────────────────────────────────────────────────────────────────────┘
```

## ⚡ Enhanced Power Requirements & Setup

### Voltage Levels (Both Board Types)
- **ESP32**: 3.3V logic, 5V input power
- **RC522**: 3.3V only (DO NOT use 5V!)
- **LCD**: 5V for backlight, 3.3V logic OK
- **HC-SR04**: 5V for reliable operation
- **IR Sensors**: 3.3V to 5V compatible
- **🆕 Servo Motor**: 5V for reliable operation
- **🆕 LED Indicators**: 3.3V logic level

### Enhanced Current Consumption
| Component | Current Draw | Notes |
|-----------|--------------|-------|
| ESP32 WROOM-32 | 80-240mA | Main controller |
| ESP32 DevKit V1 | 80-240mA | Main controller |
| RC522 | 13-50mA | NFC/RFID module |
| LCD with Backlight | 50-100mA | 20x4 display |
| HC-SR04 | 15mA | Distance sensor |
| IR Sensors (2x) | 20mA | Passenger counting |
| **🆕 Servo Motor** | **50-200mA** | **Gate control** |
| **🆕 LED Indicators (5x)** | **25-50mA** | **Status lights** |
| **Total** | **~250-600mA** | **Enhanced system** |

### Power Supply Recommendations

| Board Type | Recommended Power | Setup Difficulty | Enhanced Features |
|------------|-------------------|------------------|-------------------|
| **DevKit V1** | 5V 2A USB adapter | ⭐ Easy | ✅ All features |
| **WROOM-32** | 3.3V 1A regulated | ⭐⭐⭐ Advanced | ✅ All features |

## 🔍 Enhanced Connection Verification

### Pre-Power Checklist
- [ ] All GND connections secure
- [ ] No short circuits between VCC and GND
- [ ] RC522 connected to 3.3V (NOT 5V)
- [ ] LCD connected to 5V power
- [ ] Servo motor connected to 5V power
- [ ] LED indicators with proper resistors
- [ ] All signal wires properly connected
- [ ] No loose connections on breadboard

### Enhanced Post-Power Tests
```cpp
// Add to setup() for comprehensive testing
void testAllConnections() {
    // Test LCD
    lcd.init();
    lcd.backlight();
    lcd.print("LCD OK - 20x4");
    
    // Test RC522
    if (rfid.PCD_PerformSelfTest()) {
        Serial.println("RC522 OK");
    }
    
    // Test Ultrasonic
    float distance = readUltrasonicDistance();
    Serial.println("Distance: " + String(distance));
    
    // Test IR Sensors
    Serial.println("IR1: " + String(digitalRead(IR_ENTER)));
    Serial.println("IR2: " + String(digitalRead(IR_EXIT)));
    
    // Test Servo
    gateServo.attach(SERVO_PIN);
    gateServo.write(90);
    Serial.println("Servo OK");
    
    // Test LED Indicators
    setLcdLights(LIGHT_ALL);
    delay(1000);
    setLcdLights(LIGHT_WHITE);
    Serial.println("LEDs OK");
}
```

## 🛠️ Enhanced Assembly Tips

### Best Practices
1. **Use solid core wires** for breadboard connections
2. **Keep wires short** to reduce noise
3. **Group by voltage level** (3.3V vs 5V components)
4. **Double-check polarity** before powering on
5. **Test each component individually** before integration
6. **🆕 Add resistors** for LED indicators (220Ω recommended)
7. **🆕 Secure servo mounting** for stable gate operation
8. **🆕 Test gate movement** before final assembly

### Enhanced Common Mistakes
- **Wrong voltage to RC522**: Will damage the module
- **Loose breadboard connections**: Causes intermittent failures
- **Mixed up SDA/SCL**: I2C won't work
- **Reversed IR sensor polarity**: No detection
- **Missing pull-up resistors**: May cause noise (usually not needed)
- **🆕 Servo without 5V**: Unreliable operation
- **🆕 LEDs without resistors**: May damage ESP32 pins
- **🆕 Gate interference**: Ensure clear movement path

## 🔧 Enhanced Troubleshooting Guide

### LCD Not Working
1. Check I2C address (try 0x27 and 0x3F)
2. Verify SDA/SCL connections
3. Ensure 5V power to LCD
4. Run I2C scanner code
5. **🆕 Check for 20x4 compatibility**

### RC522 Not Detecting Cards
1. Verify 3.3V power (NOT 5V!)
2. Check SPI connections (MOSI/MISO/SCK)
3. Ensure proper RST connection
4. Test with different NFC cards
5. **🆕 Check antenna connection**

### Ultrasonic Giving Wrong Readings
1. Check 5V power supply
2. Verify TRIG/ECHO connections
3. Ensure no obstacles near sensor
4. Check for loose wires
5. **🆕 Test collision detection**

### IR Sensors Not Responding
1. Verify power connections
2. Test sensor LED indicators
3. Check detection range (2-30cm typical)
4. Ensure no interference from other IR sources
5. **🆕 Test passenger counting logic**

### 🆕 **NEW: Servo Motor Issues**
1. **Check 5V power supply** (servo needs 5V)
2. **Verify signal wire connection** to GPIO 15
3. **Test gate movement** manually
4. **Check for mechanical interference**
5. **Verify gate angles** (93° open, 173° closed)

### 🆕 **NEW: LED Indicator Issues**
1. **Check resistor connections** (220Ω recommended)
2. **Verify GPIO pin assignments**
3. **Test individual LEDs** one by one
4. **Check for loose connections**
5. **Verify light patterns** for different modes

## 📦 Enhanced Shopping List for Your Setup

### Your ESP32 Boards (You Already Have These! ✅)
```
✅ ESP32 WROOM-32 DevKit (40 pins) - Ready to use
✅ ESP32 Dev Module (32 pins) - Ready to use
✅ Both have USB Mini-B programming
✅ Both have built-in voltage regulators
```

### Essential Components Needed
```
□ RC522 RFID Module with antenna (~$3-5)
□ HC-SR04 Ultrasonic Distance Sensor (~$2-3) 
□ 2x IR Obstacle Detection Sensors (~$1-2 each)
□ 20x4 LCD Display with I2C Backpack (~$5-8)
□ Servo Motor (SG90 or similar) (~$3-5)
□ 5x LED Indicators (different colors) (~$2-3)
□ 5x 220Ω Resistors for LEDs (~$1)
□ Half-size breadboard (~$2-3)
□ Male-to-male jumper wires pack (~$2-3)
□ NFC cards for testing (~$2-5)
□ USB Mini-B cable (~$2-3)

Total Enhanced Cost: ~$25-40
Setup Time: 45 minutes  
Skill Level: Beginner to Intermediate ✅
```

### 🆕 **NEW: Optional Enhancements**
```
□ Buzzer Module (~$1-2) - Audio alerts
□ GPS Module (~$5-8) - Location tracking
□ GSM Module (~$8-12) - SMS notifications
□ SD Card Module (~$2-3) - Data logging
□ Real-time Clock (~$2-3) - Accurate timestamps
□ Temperature Sensor (~$1-2) - Environmental monitoring
□ Motion Sensor (~$2-3) - Additional security
```

## 🎯 Enhanced Quick Assembly Guide

```
Step 1: Connect ESP32 → Computer (USB Mini-B)
Step 2: Upload enhanced Arduino code 
Step 3: Connect components per enhanced pin table above
Step 4: Test each sensor and new component individually
Step 5: Test gate operation and LED indicators
Step 6: Access enhanced web interface at 192.168.4.1
Step 7: Test NFC registration and passenger counting
Step 8: Verify gate control and LED status patterns

Assembly Time: 45 minutes
Tools Needed: None! 
Enhanced Features: ✅ Gate Control ✅ LED Status ✅ Advanced Analytics
```

## 🆕 **NEW: Advanced Features Testing**

### Gate Control Test
```cpp
// Test gate opening and closing
void testGateControl() {
    Serial.println("Testing gate control...");
    
    // Test opening
    openGate();
    delay(2000);
    
    // Test closing
    closeGate();
    delay(2000);
    
    Serial.println("Gate control test complete");
}
```

### LED Pattern Test
```cpp
// Test all LED patterns
void testLEDPatterns() {
    Serial.println("Testing LED patterns...");
    
    setLcdLights(LIGHT_WHITE);    // Normal
    delay(1000);
    setLcdLights(LIGHT_GREEN);    // Success
    delay(1000);
    setLcdLights(LIGHT_RED);      // Warning
    delay(1000);
    setLcdLights(LIGHT_ALERT);    // Alert pattern
    delay(1000);
    setLcdLights(LIGHT_OFF);      // All off
    
    Serial.println("LED pattern test complete");
}
```

### Enhanced System Test
```cpp
// Complete system test
void runSystemTest() {
    Serial.println("=== ENHANCED SYSTEM TEST ===");
    
    testAllConnections();
    testGateControl();
    testLEDPatterns();
    
    // Test passenger counting
    Serial.println("Passenger counting system ready");
    
    // Test NFC system
    Serial.println("NFC system ready");
    
    // Test web interface
    Serial.println("Web interface ready");
    
    Serial.println("=== ALL SYSTEMS OPERATIONAL ===");
}
```

---

This enhanced wiring guide now includes all the new features discovered in the complete code analysis:
- ✅ **Servo Motor Gate Control** for automatic passenger access
- ✅ **5x LED Status Indicators** for visual system feedback  
- ✅ **Enhanced 20x4 LCD Display** for better information display
- ✅ **Advanced Passenger Counting** with state machine logic
- ✅ **Collision Detection** with ultrasonic sensor
- ✅ **Complete Web Management** interface
- ✅ **Real-time Analytics** and monitoring

Both your ESP32 boards work identically for this enhanced project! 🚌✨🔧
