# ESP32 Smart Bus System - Complete IoT Solution

## 🚌 Overview

The ESP32 Smart Bus System is a comprehensive, standalone IoT solution that transforms any ESP32 development board into a fully functional smart bus management system. This system integrates multiple sensors, NFC/RFID technology, web-based management, and advanced analytics to provide a complete public transportation solution.

## ✨ Key Features

### 🎫 **Smart Ticketing System**
- **NFC/RFID Card Reading**: Supports MIFARE Classic and other NFC cards
- **User Registration**: Automatic user creation with NFC card scanning
- **Balance Management**: Real-time balance tracking and fare deduction
- **Trip Management**: Automatic trip start/end with distance-based fare calculation
- **User Types**: Support for Students (50% discount), Senior Citizens (35% discount), General users, Drivers, and Police

### 📊 **Advanced Analytics & Monitoring**
- **Real-time Dashboard**: Live web interface with system status
- **Revenue Tracking**: Comprehensive financial analytics and reporting
- **Trip Analytics**: Distance, duration, and fare statistics
- **User Analytics**: Usage patterns and demographic analysis
- **Performance Metrics**: System health, memory usage, and uptime monitoring

### 🚦 **Safety & Control Systems**
- **Speed Monitoring**: Real-time speed tracking with configurable limits
- **Emergency Systems**: Emergency stop functionality and alert systems
- **Passenger Counting**: IR sensor-based automatic passenger tracking
- **Gate Control**: Servo motor-controlled access gates
- **Collision Prevention**: Ultrasonic distance sensing for obstacle detection

### 🌐 **Web Management Interface**
- **Responsive Design**: Mobile-friendly web interface
- **Real-time Updates**: Live data refresh every 3 seconds
- **User Management**: Add, edit, delete, and block users
- **Route Management**: Dynamic route selection and monitoring
- **System Controls**: Emergency stop, alerts, and system reset

### 🔧 **Hardware Integration**
- **Multi-Sensor Support**: NFC, IR, Ultrasonic, LCD, Servo, LED indicators
- **ESP32 Compatibility**: Works with ESP32 WROOM-32 DevKit and ESP32 Dev Module
- **Persistent Storage**: Data saved to ESP32's non-volatile memory
- **WiFi Access Point**: Creates its own network for easy access

## 🏗️ System Architecture

### **Core Components**
1. **ESP32 Microcontroller**: Main processing unit with WiFi and Bluetooth
2. **RC522 NFC Module**: RFID card reading and user identification
3. **HC-SR04 Ultrasonic Sensor**: Distance measurement and collision detection
4. **IR Sensors (2x)**: Passenger entry/exit detection
5. **20x4 LCD Display**: Real-time system information display
6. **Servo Motor**: Gate control for passenger access
7. **LED Indicators**: Multi-color status and alert indication
8. **Web Server**: Built-in HTTP server for remote management

### **Data Structures**
- **User Management**: Comprehensive user profiles with balance and trip history
- **Trip Tracking**: Real-time trip monitoring with start/end locations
- **Route Management**: Multi-route support with stop information
- **System Alerts**: Configurable alert system with severity levels
- **Analytics Data**: Performance metrics and business intelligence

## 🚀 Getting Started

### **Prerequisites**
- ESP32 development board (WROOM-32 DevKit or Dev Module)
- Arduino IDE with ESP32 board support
- Required libraries (see Dependencies section)
- Hardware components (see Hardware Requirements)

### **Installation**
1. Clone or download the project files
2. Install required Arduino libraries
3. Connect hardware components according to wiring guide
4. Upload the code to your ESP32
5. Power on and connect to the WiFi network
6. Access the web interface at the displayed IP address

### **Quick Setup**
```cpp
// The system automatically:
// 1. Creates WiFi access point "SmartBus_WiFi" (password: 12345678)
// 2. Initializes all sensors and displays
// 3. Loads default user data
// 4. Starts web server
// 5. Begins monitoring and data collection
```

## 🔌 Hardware Requirements

### **Essential Components**
| Component | Quantity | Purpose | Approximate Cost |
|-----------|----------|---------|------------------|
| ESP32 Dev Board | 1 | Main controller | $8-15 |
| RC522 NFC Module | 1 | Card reading | $3-5 |
| HC-SR04 Ultrasonic | 1 | Distance sensing | $2-3 |
| IR Obstacle Sensors | 2 | Passenger counting | $2-4 |
| 20x4 LCD with I2C | 1 | Status display | $5-8 |
| Servo Motor | 1 | Gate control | $3-5 |
| LED Indicators | 5 | Status lights | $2-3 |
| Breadboard | 1 | Prototyping | $2-3 |
| Jumper Wires | 20+ | Connections | $2-3 |

**Total Estimated Cost: $30-50**

### **Optional Enhancements**
- **Buzzer Module**: Audio alerts and notifications
- **GPS Module**: Real-time location tracking
- **GSM Module**: SMS notifications and remote monitoring
- **SD Card Module**: Extended data logging
- **Real-time Clock**: Accurate timestamp recording

## 📱 Web Interface Features

### **Main Dashboard**
- **Real-time Status**: Live passenger count, bus position, and speed
- **Route Progress**: Visual route progress bar with stop indicators
- **System Health**: Memory usage, uptime, and connection status
- **Quick Controls**: Emergency stop, alerts, and system functions

### **User Management**
- **User Registration**: NFC-based automatic user creation
- **Profile Management**: Edit user details, balance, and status
- **User Blocking**: Temporary or permanent user suspension
- **Balance Operations**: Add funds and view transaction history

### **Trip Management**
- **Active Trips**: Real-time monitoring of ongoing journeys
- **Trip History**: Complete record of completed trips
- **Fare Calculation**: Distance-based pricing with user discounts
- **Trip Analytics**: Performance metrics and revenue analysis

### **System Administration**
- **Route Selection**: Dynamic route switching and management
- **Alert Management**: System alerts and notification control
- **Data Management**: Backup, restore, and system reset
- **Performance Monitoring**: System health and optimization

## 🔧 Configuration & Customization

### **System Parameters**
```cpp
// Fare Configuration
const float BASE_RATE_PER_KM = 5.0;           // Base fare per kilometer
const float STUDENT_DISCOUNT = 0.5;           // 50% student discount
const float SENIOR_DISCOUNT = 0.35;           // 35% senior discount
const float PEAK_HOUR_MULTIPLIER = 1.5;       // Peak hour pricing

// Safety Limits
const float MAX_SPEED_LIMIT = 40.0;           // Maximum speed in km/h
const float SPEED_WARNING_THRESHOLD = 20.0;   // Speed warning threshold
const int MAX_PASSENGER_LIMIT = 5;            // Maximum passengers

// Timing Configuration
const unsigned long NFC_DEBOUNCE_TIME = 1500;  // NFC response delay
const unsigned long GATE_AUTO_CLOSE_TIME = 10000; // Gate auto-close delay
```

### **Route Configuration**
```cpp
// Example route structure
struct BusRoute {
    String id;                    // Route identifier
    String name;                  // Route name
    float totalDistance;          // Total route distance
    float farePerKm;             // Fare per kilometer
    String color;                 // Route color for UI
    int maxSpeed;                 // Maximum allowed speed
    std::vector<RouteStop> stops; // Route stops with coordinates
};
```

### **User Type Configuration**
```cpp
// Supported user types with automatic discount application
enum UserTypes {
    "General",           // Standard fare
    "Student",           // 50% discount
    "Senior Citizen",    // 35% discount
    "Driver",            // Special access
    "Police",            // Emergency access
    "Admin"              // System administration
};
```

## 📊 API Endpoints

### **System Status & Control**
- `GET /api/status` - System status and sensor data
- `GET /api/health` - System health and diagnostics
- `GET /api/stats` - System statistics and metrics
- `POST /api/emergency_stop` - Activate emergency stop
- `POST /api/clear_alerts` - Clear system alerts

### **User Management**
- `GET /api/users` - List all registered users
- `POST /api/register_user` - Register new user
- `POST /api/edit_user` - Update user information
- `POST /api/delete_user` - Remove user
- `POST /api/add_balance` - Add funds to user account
- `POST /api/block_user` - Block/unblock user

### **Trip Management**
- `GET /api/trips` - Active trips information
- `GET /api/trip_events` - Trip event notifications
- `POST /api/rate_trip` - Rate completed trip
- `GET /api/analytics` - Trip and revenue analytics

### **Route Management**
- `GET /api/routes` - Available routes and information
- `POST /api/select_route` - Change active route
- `GET /api/bus` - Current bus status and position

### **NFC Operations**
- `POST /api/start_nfc_registration` - Start NFC card reading
- `GET /api/get_nfc_registration_status` - Check NFC status
- `POST /api/cancel_nfc_registration` - Cancel NFC operation

## 🚨 Safety Features

### **Speed Monitoring**
- **Real-time Speed Tracking**: Continuous speed measurement
- **Configurable Limits**: Adjustable speed thresholds
- **Warning Systems**: Visual and audio speed warnings
- **Emergency Response**: Automatic emergency stop for dangerous speeds

### **Collision Prevention**
- **Distance Sensing**: Ultrasonic sensor for obstacle detection
- **Automatic Braking**: Emergency stop when obstacles detected
- **Warning Systems**: Visual alerts for potential collisions
- **Safety Margins**: Configurable safety distances

### **Passenger Safety**
- **Capacity Monitoring**: Automatic passenger limit enforcement
- **Access Control**: Gate control for safe boarding/alighting
- **Emergency Systems**: Quick emergency response capabilities
- **Status Monitoring**: Continuous system health monitoring

## 📈 Analytics & Reporting

### **Business Intelligence**
- **Revenue Analytics**: Daily, weekly, and monthly revenue tracking
- **Trip Analytics**: Distance, duration, and fare analysis
- **User Analytics**: Usage patterns and demographic insights
- **Route Performance**: Route efficiency and profitability analysis

### **Performance Metrics**
- **System Uptime**: Continuous operation monitoring
- **Response Times**: Sensor and API response performance
- **Memory Usage**: System resource utilization
- **Error Tracking**: System error and alert monitoring

### **Export Capabilities**
- **Data Export**: CSV and JSON data export
- **Report Generation**: Automated report creation
- **Historical Data**: Long-term data storage and analysis
- **Backup Systems**: Automatic data backup and recovery

## 🔒 Security Features

### **Access Control**
- **User Authentication**: NFC-based user identification
- **Role-based Access**: Different permissions for user types
- **Account Blocking**: Temporary and permanent account suspension
- **Transaction Logging**: Complete audit trail of all operations

### **Data Protection**
- **Persistent Storage**: Secure data storage in ESP32 memory
- **Data Validation**: Input validation and sanitization
- **Error Handling**: Comprehensive error handling and recovery
- **Backup Systems**: Automatic data backup and recovery

## 🛠️ Troubleshooting

### **Common Issues**

#### **NFC Not Working**
- Check RC522 power connection (3.3V only)
- Verify SPI pin connections
- Test with different NFC cards
- Check antenna connection

#### **LCD Display Issues**
- Verify I2C address (try 0x27 or 0x3F)
- Check power connections (5V for backlight)
- Ensure proper SDA/SCL connections
- Test with I2C scanner

#### **Sensor Problems**
- Check power supply connections
- Verify signal wire connections
- Test individual sensors
- Check for interference

#### **WiFi Connection Issues**
- Verify WiFi credentials in code
- Check ESP32 WiFi capabilities
- Ensure proper power supply
- Reset ESP32 if needed

### **Debug Information**
```cpp
// Enable debug output
Serial.begin(115200);

// Check system status
Serial.println("Free Memory: " + String(ESP.getFreeHeap()));
Serial.println("WiFi Status: " + String(WiFi.status()));
Serial.println("Sensor Readings: " + String(sensorReadings));
```

## 📚 Dependencies

### **Required Libraries**
```cpp
#include <WiFi.h>              // WiFi functionality
#include <WebServer.h>          // Web server
#include <ArduinoJson.h>        // JSON handling
#include <Wire.h>               // I2C communication
#include <LiquidCrystal_I2C.h>  // LCD control
#include <SPI.h>                // SPI communication
#include <MFRC522.h>            // NFC/RFID
#include <Preferences.h>         // Persistent storage
#include <ESP32Servo.h>         // Servo motor control
```

### **Library Installation**
1. Open Arduino IDE
2. Go to Tools → Manage Libraries
3. Search and install each required library
4. Restart Arduino IDE

## 🚀 Performance Optimization

### **Memory Management**
- **Dynamic Memory Allocation**: Efficient memory usage
- **Garbage Collection**: Automatic memory cleanup
- **Data Structures**: Optimized data organization
- **Response Caching**: Reduced memory allocation

### **Response Time Optimization**
- **Asynchronous Operations**: Non-blocking operations
- **Request Throttling**: Rate limiting for stability
- **Efficient Algorithms**: Optimized data processing
- **Background Tasks**: Non-critical operations in background

### **Power Management**
- **Sleep Modes**: Power-saving when idle
- **Efficient Sensors**: Low-power sensor operation
- **Optimized Loops**: Reduced CPU usage
- **Smart Timing**: Intelligent operation scheduling

## 🔮 Future Enhancements

### **Planned Features**
- **GPS Integration**: Real-time location tracking
- **Mobile App**: Native mobile application
- **Cloud Integration**: Remote data storage and analytics
- **AI Integration**: Predictive analytics and optimization
- **Multi-language Support**: Internationalization
- **Advanced Security**: Encryption and authentication

### **Scalability Improvements**
- **Multi-bus Support**: Fleet management capabilities
- **Distributed Systems**: Multiple ESP32 coordination
- **Load Balancing**: Improved performance under load
- **Database Integration**: External database support

## 📄 License

This project is open source and available under the MIT License. See the LICENSE file for details.

## 🤝 Contributing

Contributions are welcome! Please feel free to submit pull requests or open issues for bugs and feature requests.

### **Development Guidelines**
- Follow existing code style
- Add comprehensive documentation
- Include test cases
- Ensure backward compatibility
- Update README for new features

## 📞 Support

For support and questions:
- Create an issue on GitHub
- Check the troubleshooting section
- Review the wiring guide
- Test with minimal hardware setup

## 🙏 Acknowledgments

- ESP32 development community
- Arduino ecosystem contributors
- Open source library developers
- Hardware component manufacturers

---

**Built with ❤️ for the IoT and transportation communities**

*This system demonstrates the power of ESP32 for complex IoT applications, providing a complete solution for smart transportation management.*
