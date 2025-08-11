# ESP32 Smart Bus - Wiring Diagram & Hardware Guide

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
```

## 📋 Pin Assignment Table (Your Boards)

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
```

### LCD Display (16x2 with I2C Backpack)
```
LCD I2C Module:
┌─────────────────┐
│ GND VCC SDA SCL │
└─────────────────┘

Connections:
LCD GND → ESP32 GND
LCD VCC → ESP32 VIN (5V)
LCD SDA → ESP32 GPIO 21
LCD SCL → ESP32 GPIO 22

Note: I2C Address usually 0x27 or 0x3F
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
```

## 🍞 Breadboard Layout (Works with Both Your Boards)

### ESP32 Smart Bus - Complete Layout
```
Breadboard Power Rails:
Red Rail (+)  → ESP32 VIN (5V from USB)
Blue Rail (-) → ESP32 GND

Component Placement:
┌─────────────────────────────────────────────────────────────┐
│ (+) ════════════════════════════════════════════════════ RED│
│ (-) ════════════════════════════════════════════════════BLUE│
│                                                             │
│     ┌─ESP32 Board─┐              ┌─LCD I2C─┐               │
│     │  (Either    │              │ GND VCC │               │  
│     │   Board)    │              │ SDA SCL │               │
│     │             │              └─────────┘               │
│     │ G5  G18 G23 │    ┌─RC522 RFID─┐                     │
│     │ G19 G4  G21 │    │ SDA SCK MOSI│                     │
│     │ G22 G26 G27 │    │ MISO RST GND│                     │
│     │ G32 G33     │    │ IRQ  3.3V   │                     │
│     │ 3V3 GND VIN │    └─────────────┘                     │
│     └─────────────┘                                        │
│                           ┌─HC-SR04─┐    ┌─IR1─┐ ┌─IR2─┐  │
│                           │VCC TRIG │    │VCC  │ │VCC  │  │
│                           │ECHO GND │    │GND  │ │GND  │  │
│                           └─────────┘    │OUT  │ │OUT  │  │
│                                          └─────┘ └─────┘  │
│ (+) ════════════════════════════════════════════════════ RED│
│ (-) ════════════════════════════════════════════════════BLUE│
└─────────────────────────────────────────────────────────────┘
```
│ + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + │ ← 5V
│                                                             │
│   ┌─────┐     ┌─────────┐         ┌─────────────────┐       │
│   │ IR1 │     │  RC522  │         │   ESP32 DevKit  │       │
│   └─────┘     └─────────┘         │       V1        │       │
│                                   └─────────────────┘       │
│   ┌─────┐     ┌─────────┐                                   │
│   │ IR2 │     │ HC-SR04 │           ┌─────────────┐         │
│   └─────┘     └─────────┘           │  LCD I2C    │         │
│                                     └─────────────┘         │
│                                                             │
│ - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - │ ← GND
└─────────────────────────────────────────────────────────────┘
```

### Option 2: ESP32 WROOM-32 Layout (Advanced)
```
Breadboard Power Rails:
Red Rail (+)  → 5V Input → Voltage Regulator → 3.3V
Blue Rail (-) → Common GND

Component Placement:
┌─────────────────────────────────────────────────────────────┐
│ + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + │ ← 5V
│                                                             │
│ ┌─────────┐   ┌─────┐     ┌─────────┐   ┌───────────────┐   │
│ │ AMS1117 │   │ IR1 │     │  RC522  │   │ ESP32 WROOM32 │   │
│ │ 3.3V Reg│   └─────┘     └─────────┘   │  on Breakout  │   │
│ └─────────┘                             └───────────────┘   │
│                                                             │
│ ┌─────────┐   ┌─────┐     ┌─────────┐   ┌─────────────┐     │
│ │USB-Serial│   │ IR2 │     │ HC-SR04 │   │  LCD I2C    │     │
│ │Converter │   └─────┘     └─────────┘   └─────────────┘     │
│ └─────────┘                                                 │
│                                                             │
│ - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - │ ← GND
└─────────────────────────────────────────────────────────────┘
```

## ⚡ Power Requirements & Setup

### Voltage Levels (Both Board Types)
- **ESP32**: 3.3V logic, 5V input power
- **RC522**: 3.3V only (DO NOT use 5V!)
- **LCD**: 5V for backlight, 3.3V logic OK
- **HC-SR04**: 5V for reliable operation
- **IR Sensors**: 3.3V to 5V compatible

### Current Consumption
| Component | Current Draw |
|-----------|--------------|
| ESP32 WROOM-32 | 80-240mA |
| ESP32 DevKit V1 | 80-240mA |
| RC522 | 13-50mA |
| LCD with Backlight | 50-100mA |
| HC-SR04 | 15mA |
| IR Sensors (2x) | 20mA |
| **Total** | **~200-400mA** |

### Power Supply Options

#### For ESP32 DevKit V1 (Simple):
- **USB Power**: 5V 2A USB adapter → DevKit USB port
- **External**: 5V → VIN pin, GND → GND pin
- **Battery**: 7.4V Li-Po → VIN (built-in regulator)

#### For ESP32 WROOM-32 (Complex):
- **Regulated 3.3V**: External 3.3V supply → VCC pin
- **5V with Regulator**: 5V → AMS1117-3.3V → WROOM VCC
- **Battery**: 7.4V Li-Po → Buck converter → 3.3V

### Power Supply Recommendations

| Board Type | Recommended Power | Setup Difficulty |
|------------|-------------------|------------------|
| **DevKit V1** | 5V 2A USB adapter | ⭐ Easy |
| **WROOM-32** | 3.3V 1A regulated | ⭐⭐⭐ Advanced |

## 🔍 Connection Verification

### Pre-Power Checklist
- [ ] All GND connections secure
- [ ] No short circuits between VCC and GND
- [ ] RC522 connected to 3.3V (NOT 5V)
- [ ] LCD connected to 5V power
- [ ] All signal wires properly connected
- [ ] No loose connections on breadboard

### Post-Power Tests
```cpp
// Add to setup() for testing
void testConnections() {
    // Test LCD
    lcd.init();
    lcd.backlight();
    lcd.print("LCD OK");
    
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
}
```

## 🛠️ Assembly Tips

### Best Practices
1. **Use solid core wires** for breadboard connections
2. **Keep wires short** to reduce noise
3. **Group by voltage level** (3.3V vs 5V components)
4. **Double-check polarity** before powering on
5. **Test each component individually** before integration

### Common Mistakes
- **Wrong voltage to RC522**: Will damage the module
- **Loose breadboard connections**: Causes intermittent failures
- **Mixed up SDA/SCL**: I2C won't work
- **Reversed IR sensor polarity**: No detection
- **Missing pull-up resistors**: May cause noise (usually not needed)

## 🔧 Troubleshooting Guide

### LCD Not Working
1. Check I2C address (try 0x27 and 0x3F)
2. Verify SDA/SCL connections
3. Ensure 5V power to LCD
4. Run I2C scanner code

### RC522 Not Detecting Cards
1. Verify 3.3V power (NOT 5V!)
2. Check SPI connections (MOSI/MISO/SCK)
3. Ensure proper RST connection
4. Test with different NFC cards

### Ultrasonic Giving Wrong Readings
1. Check 5V power supply
2. Verify TRIG/ECHO connections
3. Ensure no obstacles near sensor
4. Check for loose wires

### IR Sensors Not Responding
1. Verify power connections
2. Test sensor LED indicators
3. Check detection range (2-30cm typical)
4. Ensure no interference from other IR sources

## 📦 Shopping List for Your Setup

### Your ESP32 Boards (You Already Have These! ✅)
```
✅ ESP32 WROOM-32 DevKit (40 pins) - Ready to use
✅ ESP32 Dev Module (32 pins) - Ready to use
✅ Both have USB Mini-B programming
✅ Both have built-in voltage regulators
```

### Additional Components Needed
```
□ RC522 RFID Module with antenna (~$3-5)
□ HC-SR04 Ultrasonic Distance Sensor (~$2-3) 
□ 2x IR Obstacle Detection Sensors (~$1-2 each)
□ 16x2 LCD Display with I2C Backpack (~$3-4)
□ Half-size breadboard (~$2-3)
□ Male-to-male jumper wires pack (~$2-3)
□ NFC cards for testing (~$2-5)
□ USB Mini-B cable (~$2-3)

Total Additional Cost: ~$20-30
Setup Time: 30 minutes  
Skill Level: Beginner ✅
```

## 🎯 Quick Assembly Guide

```
Step 1: Connect ESP32 → Computer (USB Mini-B)
Step 2: Upload Arduino code 
Step 3: Connect components per pin table above
Step 4: Test each sensor individually
Step 5: Access web interface at 192.168.4.1

Assembly Time: 30 minutes
Tools Needed: None! 
```

---

This simplified wiring guide focuses on your specific ESP32 boards and eliminates complexity. Both your boards work identically for this project! 🚌✨
