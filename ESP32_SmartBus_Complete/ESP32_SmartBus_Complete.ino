/*
 * ESP32 Complete Smart Bus System - ENHANCED VERSION
 *
 * This is a comprehensive standalone ESP32 implementation featuring ALL IoT project capabilities:
 * - NFC/RFID card reading with advanced UID processing (RC522)
 * - Ultrasonic distance sensor with speed calculation (HC-SR04)
 * - IR passenger counting with sophisticated state machine (2x sensors)
 * - LCD display with rotating status modes
 * - Advanced web server with responsive HTML/CSS/JS frontend
 * - WiFi AP with connection monitoring
 * - Complete bus ticketing system with trip management
 * - Multi-route support with dynamic switching
 * - User registration system with balance management
 * - Real-time analytics and monitoring
 * - Emergency and alert systems
 * - Police monitoring capabilities
 * - Driver dashboard functionality
 * - Advanced fare calculation with discounts
 * - Performance optimization and error handling
 *
 * 🔧 Board Compatibility:
 * ✅ ESP32 WROOM-32 DevKit (40 pins) - Primary board with USB mini port
 * ✅ ESP32 Dev Module (32 pins) - Secondary board with USB mini port
 *
 * 
 * Hardware Connections (GPIO numbers same for both boards):
 * 
 * RC522 RFID Module:
 * - VCC → 3.3V, GND → GND
 * - SDA → GPIO 5, SCK → GPIO 18
 * - MOSI → GPIO 23, MISO → GPIO 19, RST → GPIO 4
 *
 * LCD Display (I2C):
 * - VCC → 5V, GND → GND
 * - SDA → GPIO 21, SCL → GPIO 22
 *
 * HC-SR04 Ultrasonic:
 * - VCC → 5V, GND → GND
 * - Trig → GPIO 26, Echo → GPIO 27
 *
 * IR Sensors:
 * - IR1 (Entry) → GPIO 32, IR2 (Exit) → GPIO 33
 * - VCC → 3.3V, GND → GND
 * 
 * Servo Motor:
 * - VCC → 5V, GND → GND
 * - Signal → GPIO 15
 * 
 * Bulb:
 * - GND → GND (via resistor)
 * - Signal → GPIO 25, GPIO 13, GPIO 12, GPIO 14, GPIO 2
 *
 * ⚠️ Power Requirements:
 * - Both boards: 5V via USB mini port or VIN pin
 * - Both have built-in voltage regulators and USB programming
 *
 * 🌟 Enhanced Features:
 * - Real-time performance monitoring
 * - Advanced analytics dashboard
 * - Emergency alert system
 * - Speed limit monitoring
 * - Traffic violation detection
 * - Multi-user role support (User/Driver/Admin/Police)
 * - Comprehensive trip tracking
 * - Revenue analytics
 * - System health monitoring
 * - Automatic error recovery
 * - Enhanced security features
 *
 * Author: Smart Bus System - Enhanced Edition
 * Date: August 2025
 * Version: Complete ESP32 Standalone - ALL FEATURES INCLUDED
 */

 #include <WiFi.h>
 #include <WebServer.h>
 #include <ArduinoJson.h>
 #include <Wire.h>
 #include <LiquidCrystal_I2C.h>
 #include <SPI.h>
 #include <MFRC522.h>
 #include <Preferences.h>
 #include <EEPROM.h>
 #include <ESP32Servo.h>
 
 // Pin Definitions
 #define RC522_SS_PIN 5
 #define RC522_RST_PIN 4
 #define LCD_SDA 21
 #define LCD_SCL 22
 #define TRIG_PIN 26
 #define ECHO_PIN 27
 #define IR_ENTER 32
 #define IR_EXIT 33
 
 // New LCD Light and Servo Motor Pins for ESP32
 #define LCD_LIGHT_WHITE 25   
 #define LCD_LIGHT_GREEN_1 13
 #define LCD_LIGHT_GREEN_2 12
 #define LCD_LIGHT_RED_1 14
 #define LCD_LIGHT_RED_2 2
 
 // Servo Motor Configuration
 #define SERVO_PIN 15 // Servo motor pin for gate control
 #define SERVO_OPEN_ANGLE 93   // Angle for open gate
 #define SERVO_CLOSE_ANGLE 174 // Angle for closed gate
 
 // Network Configuration
 const char *ssid = "SmartBus_WiFi";
 const char *password = "12345678";
 
 // Hardware Objects
 MFRC522 rfid(RC522_SS_PIN, RC522_RST_PIN);
 LiquidCrystal_I2C lcd(0x27, 20, 4);
 WebServer server(80);
 Preferences preferences; // For persistent data storage
 Servo gateServo;         // Servo object for gate control
 
 // Servo/Gate Control Variables
 bool gateOpen = false; // Gate starts CLOSED
 unsigned long lastGateAction = 0;
 const unsigned long GATE_ACTION_DELAY = 1000;     // 1 second delay between gate actions
 unsigned long gateOpenTime = 0;                   // When gate was opened
 bool gateAutoCloseEnabled = false;                // Auto-close timer active
 const float MIN_BALANCE_FOR_ENTRY = 10.0;         // Minimum 10 Tk balance required
 const int SERVO_PWM_CHANNEL = 0;                  // PWM channel for servo control
 
 // System Data Structures - Enhanced
 struct User
 {
     String uid;
     String name;
     String userType; // Student, Senior Citizen, General, Driver, Admin, Police
     float balance;
     bool isRegistered;
     unsigned long lastTapTime;
     int totalTrips;
     float totalSpent;
     String phoneNumber;
     String address;
     bool isBlocked;
     String registrationDate;
 };
 
 struct Trip
 {
     String uid;
     String userName;
     String routeId;
     String busId;
     float startLocationKm;
     float endLocationKm;
     unsigned long startTime;
     unsigned long endTime;
     float fare;
     float distance;
     bool isActive;
     String tripStatus; // ACTIVE, COMPLETED, CANCELLED
     int rating;        // 1-5 stars
     String feedback;
 };
 
 struct RouteStop
 {
     float lat;
     float lng;
     String stopName;
     float km;
 };
 
 struct BusRoute
 {
     String id;
     String name;
     float totalDistance;
     float farePerKm;
     String color;
     int maxSpeed;
     int stopCount;
     String description;
     bool isActive;
     float totalRevenue;
     int totalTrips;
     std::vector<RouteStop> stops;
 };
 
 struct BusStatus
 {
     String routeId;
     String busId;
     float currentLocationKm;
     float currentSpeedKmph;
     int irPassengerCount;
     int tapInCount;
     String status; // IDLE, RUNNING, MAINTENANCE, EMERGENCY
     unsigned long lastUpdate;
     bool emergencyMode;
     bool speedWarning;
     bool illegalStopWarning;
     float dailyRevenue;
     int dailyTrips;
     String driverName;
     String driverID;
 };
 
 struct Alert
 {
     String type; // WARNING, ERROR, INFO, EMERGENCY
     String message;
     unsigned long timestamp;
     bool isActive;
     String severity; // LOW, MEDIUM, HIGH, CRITICAL
 };
 
 struct SystemStats
 {
     unsigned long uptime;
     int totalUsers;
     int activeTrips;
     float totalRevenue;
     int nfcReadings;
     int sensorReadings;
     int webRequests;
     float averageSpeed;
     int violationCount;
     String systemHealth; // EXCELLENT, GOOD, WARNING, CRITICAL
 };
 
 // Enhanced Global Variables
 std::vector<User> users;
 std::vector<Trip> activeTrips;
 std::vector<Trip> completedTrips;
 std::vector<BusRoute> routes;
 std::vector<Alert> systemAlerts;
 std::vector<DynamicJsonDocument> tripEvents; // For web popup events
 BusStatus currentBus;
 SystemStats stats;
 
 // NFC Registration Mode Variables
 bool nfcRegistrationMode = false;
 String lastReadUID = "";
 unsigned long nfcRegistrationTimeout = 0;
 const unsigned long NFC_REGISTRATION_TIMEOUT = 30000; // 30 seconds timeout
 
 // Enhanced Sensor Variables
 int passengerCount = 0;
 float currentDistanceCm = 0;
 unsigned long lastNfcRead = 0;
 unsigned long lastSensorRead = 0;
 unsigned long lastLcdUpdate = 0;
 unsigned long lastAnalyticsUpdate = 0;
 unsigned long lastAlertCheck = 0;
 int lcdDisplayMode = 0;
 bool systemInitialized = false;
 String lastError = "";
 unsigned long systemStartTime = 0;
 
 // Enhanced IR Sensor State Machine with Advanced Logic
 enum IRState
 {
     IDLE,
     IR1_TRIGGERED,
     IR2_TRIGGERED,
     BOTH_TRIGGERED,
     ENTRY_CONFIRMED,
     EXIT_CONFIRMED
 };
 IRState irState = IDLE;
 unsigned long irStateTime = 0;
 int pendingEntries = 0;
 int pendingExits = 0;
 
 // Enhanced Timing Constants
 const unsigned long NFC_DEBOUNCE_TIME = 1500;         // Faster NFC response
 const unsigned long SENSOR_READ_INTERVAL = 150;       // Higher sensor refresh
 const unsigned long LCD_UPDATE_INTERVAL = 4000;       // Faster LCD updates
 const unsigned long ANALYTICS_UPDATE_INTERVAL = 5000; // Analytics every 5s
 const unsigned long ALERT_CHECK_INTERVAL = 1000;      // Check alerts every 1s
 const unsigned long IR_TIMEOUT = 500;                 // Shorter IR timeout
 const unsigned long HEALTH_CHECK_INTERVAL = 10000;    // System health every 10s
 const unsigned long GATE_AUTO_CLOSE_TIME = 5000;     // 10 seconds auto-close
 
 // Enhanced Fare Constants with Dynamic Pricing
 const float BASE_RATE_PER_KM = 5.0;
 const float MIN_BALANCE = 15.0;         // Reduced minimum balance
 const float PEAK_HOUR_MULTIPLIER = 1.5; // Peak hour pricing
 const float STUDENT_DISCOUNT = 0.5;     // 50% discount
 const float SENIOR_DISCOUNT = 0.35;     // 35% discount
 const float LOYALTY_DISCOUNT = 0.1;     // 10% for frequent users
 
 // Speed and Safety Constants
 const float MAX_SPEED_LIMIT = 40.0;         // km/h
 const float SPEED_WARNING_THRESHOLD = 20.0; // km/h - Enhanced: Warning at 20 km/h
 const float EMERGENCY_STOP_THRESHOLD = 5.0; // cm for emergency stop
 const int MAX_PASSENGER_LIMIT = 2;          // Enhanced: Passenger limit for red light warning
 
 // Enhanced Network Configuration
 const int maxConnections = 10; // Support multiple clients
 
 // LCD Light and Buzzer Control Enums
 enum LightMode
 {
     LIGHT_OFF,
     LIGHT_WHITE,
     LIGHT_GREEN,
     LIGHT_RED,
     LIGHT_ALL,
     LIGHT_ALERT,
     LIGHT_SPEED_WARNING,     // Enhanced: Special mode for speed warnings
     LIGHT_PASSENGER_WARNING, // Enhanced: Special mode for passenger overflow
     LIGHT_BOARDING           // Enhanced: Special mode for passenger boarding
 };
 
 // Function Declarations
 void addTripEvent(String type, User *user, Trip *trip = nullptr, float fare = 0, float distance = 0, int duration = 0, float discount = 0);
 void setLcdLights(LightMode mode);
 void saveUserData();
 void loadUserData();
 void clearUserData();
 void initializeSystemData();
 void initializeSystemComponents();
 void handleSensors();
 float readUltrasonicDistance();
 void updateBusPosition();
 void handlePassengerCounting();
 void handleNFC();
 void handleNFCRegistration(String uid);
 void processNfcTap(String uid);
 User *findUser(String uid);
 Trip *findActiveTrip(String uid);
 void startTrip(User *user);
 void endTrip(Trip *trip, User *user);
 BusRoute *getCurrentRoute();
 String getNextStopName(float currentPosition);
 String getCurrentStopInfo(float currentPosition);
 void addAlert(String type, String message, String severity);
 void updateSystemStats();
 bool isPeakHour();
 float calculateDynamicFare(float distance, String userType, int totalTrips);
 void checkSpeedViolations();
 void performSystemHealthCheck();
 String formatTime(unsigned long timestamp);
 void cleanupOldAlerts();
 void updateLcdDisplay();
 void setupWebServer();
 String getMainHtmlPage();
 
 // Servo/Gate Control Functions
 void initializeServo();
 void openGate();
 void closeGate();
 void setServoAngle(int angle);
 void checkGateControl();
 
 // API Handler Declarations
 void handleApiStatus();
 void handleApiUsers();
 void handleApiTrips();
 void handleApiRoutes();
 void handleApiBus();
 void handleApiSelectRoute();
 void handleApiAddBalance();
 void handleApiAlerts();
 void handleApiStats();
 void handleApiAnalytics();
 void handleApiHealth();
 void handleApiEmergencyStop();
 void handleApiClearAlerts();
 void handleApiBlockUser();
 void handleApiRateTrip();
 void handleApiRegisterUser();
 void handleApiSystemReset();
 void handleApiDeleteUser();
 void handleApiEditUser();
 void handleApiSaveData();
 void handleApiClearData();
 void handleApiTripEvents();
 void handleApiStartNFCRegistration();
 void handleApiGetNFCRegistrationStatus();
 void handleApiCancelNFCRegistration();
 
 void indicateSystemStatus();

 // LCD Light and Buzzer Control Functions Implementation
 void setLcdLights(LightMode mode)
 {
     // Turn off all lights first
     digitalWrite(LCD_LIGHT_WHITE, LOW);
     digitalWrite(LCD_LIGHT_GREEN_1, LOW);
     digitalWrite(LCD_LIGHT_GREEN_2, LOW);
     digitalWrite(LCD_LIGHT_RED_1, LOW);
     digitalWrite(LCD_LIGHT_RED_2, LOW);
 
     switch (mode)
     {
     case LIGHT_WHITE:
         digitalWrite(LCD_LIGHT_WHITE, HIGH);
         break;
     case LIGHT_GREEN:
         digitalWrite(LCD_LIGHT_GREEN_1, HIGH);
         digitalWrite(LCD_LIGHT_GREEN_2, HIGH);
         break;
     case LIGHT_RED:
         digitalWrite(LCD_LIGHT_RED_1, HIGH);
         digitalWrite(LCD_LIGHT_RED_2, HIGH);
         break;
     case LIGHT_ALL:
         digitalWrite(LCD_LIGHT_WHITE, HIGH);
         digitalWrite(LCD_LIGHT_GREEN_1, HIGH);
         digitalWrite(LCD_LIGHT_GREEN_2, HIGH);
         digitalWrite(LCD_LIGHT_RED_1, HIGH);
         digitalWrite(LCD_LIGHT_RED_2, HIGH);
         break;
     case LIGHT_ALERT:
         // Flashing red pattern for alerts
         for (int i = 0; i < 3; i++)
         {
             digitalWrite(LCD_LIGHT_RED_1, HIGH);
             digitalWrite(LCD_LIGHT_RED_2, HIGH);
             delay(200);
             digitalWrite(LCD_LIGHT_RED_1, LOW);
             digitalWrite(LCD_LIGHT_RED_2, LOW);
             delay(200);
         }
         break;
     case LIGHT_SPEED_WARNING:
         // Enhanced: Rapid alternating red-white pattern for speed warning at 20 km/h
         for (int i = 0; i < 5; i++)
         {
             digitalWrite(LCD_LIGHT_RED_1, HIGH);
             digitalWrite(LCD_LIGHT_WHITE, HIGH);
             delay(150);
             digitalWrite(LCD_LIGHT_RED_1, LOW);
             digitalWrite(LCD_LIGHT_WHITE, LOW);
             digitalWrite(LCD_LIGHT_RED_2, HIGH);
             delay(150);
             digitalWrite(LCD_LIGHT_RED_2, LOW);
         }
         break;
     case LIGHT_PASSENGER_WARNING:
         // Enhanced: Blinking red lights for passenger overflow (>5 passengers)
         for (int i = 0; i < 6; i++)
         {
             digitalWrite(LCD_LIGHT_RED_1, HIGH);
             digitalWrite(LCD_LIGHT_RED_2, HIGH);
             delay(250);
             digitalWrite(LCD_LIGHT_RED_1, LOW);
             digitalWrite(LCD_LIGHT_RED_2, LOW);
             delay(250);
         }
         break;
     case LIGHT_BOARDING:
         // Enhanced: Smooth green-white transition for boarding/alighting
         digitalWrite(LCD_LIGHT_GREEN_1, HIGH);
         digitalWrite(LCD_LIGHT_GREEN_2, HIGH);
         delay(500);
         digitalWrite(LCD_LIGHT_WHITE, HIGH);
         delay(500);
         digitalWrite(LCD_LIGHT_GREEN_1, LOW);
         digitalWrite(LCD_LIGHT_GREEN_2, LOW);
         delay(300);
         digitalWrite(LCD_LIGHT_WHITE, LOW);
         break;
     case LIGHT_OFF:
     default:
         // All lights already turned off
         break;
     }
 }

 // Servo/Gate Control Functions Implementation
 void initializeServo()
 {
     // Setup servo using ESP32Servo library
     gateServo.attach(SERVO_PIN);
 
     // Start with gate CLOSED
     gateOpen = false;
     closeGate();
 
     Serial.println("✅ Servo initialized - Gate is CLOSED");
     addAlert("INFO", "Gate control system initialized - Gate CLOSED", "LOW");
 }
 
 void setServoAngle(int angle)
 {
     // Use ESP32Servo library to set servo angle
     gateServo.write(angle);
     delay(500); // Give servo time to move
 }
 
 void openGate()
 {
     if (millis() - lastGateAction < GATE_ACTION_DELAY)
         return; // Prevent rapid gate actions
 
     if (!gateOpen)
     {
         setServoAngle(SERVO_OPEN_ANGLE);
         gateOpen = true;
         lastGateAction = millis();
 
         // Visual feedback for gate opening
         setLcdLights(LIGHT_GREEN);
         delay(500);
         setLcdLights(LIGHT_WHITE);
 
         Serial.println("🚪 Gate OPENED");
         addAlert("INFO", "Gate opened - Passenger can exit", "LOW");
 
         // Update LCD to show gate status
         lcd.clear();
         lcd.setCursor(0, 0);
         lcd.print("R:" + String(ESP.getFreeHeap() / 1024) + "kb");
         lcd.setCursor(13, 0);
         lcd.print("P:" + String(passengerCount) + " T:" + String(currentBus.tapInCount));
         lcd.setCursor(0, 1);
         lcd.print("GATE OPENED");
         lcd.setCursor(0, 2);
         lcd.print("Passenger can exit");
         lcd.setCursor(0, 3);
         lcd.print("Speed: " + String(currentBus.currentSpeedKmph, 1) + " km/h");
     }
 }
 
 void closeGate()
 {
     if (millis() - lastGateAction < GATE_ACTION_DELAY)
         return; // Prevent rapid gate actions
 
     if (gateOpen)
     {
         setServoAngle(SERVO_CLOSE_ANGLE);
         gateOpen = false;
         lastGateAction = millis();
 
         // Visual feedback for gate closing
         setLcdLights(LIGHT_RED);
         delay(500);
         setLcdLights(LIGHT_WHITE);
 
         Serial.println("🚪 Gate CLOSED");
         addAlert("WARNING", "Gate closed - Access restricted", "MEDIUM");
 
         // Update LCD to show gate status
         lcd.clear();
         lcd.setCursor(0, 0);
         lcd.print("R:" + String(ESP.getFreeHeap() / 1024) + "kb");
         lcd.setCursor(13, 0);
         lcd.print("P:" + String(passengerCount) + " T:" + String(currentBus.tapInCount));
         lcd.setCursor(0, 1);
         lcd.print("GATE CLOSED");
         lcd.setCursor(0, 2);
         lcd.print("Access restricted");
         lcd.setCursor(0, 3);
         lcd.print("Speed: " + String(currentBus.currentSpeedKmph, 1) + " km/h");
     }
 }
 
 // Open gate temporarily for 10 seconds
 void openGateTemporarily(String reason)
 {
     if (millis() - lastGateAction < GATE_ACTION_DELAY)
         return; // Prevent rapid gate actions
 
     setServoAngle(SERVO_OPEN_ANGLE);
     gateOpen = true;
     gateOpenTime = millis();
     gateAutoCloseEnabled = true;
     lastGateAction = millis();
 
     // Visual feedback for gate opening
     setLcdLights(LIGHT_GREEN);
     delay(500);
     setLcdLights(LIGHT_WHITE);
 
     Serial.println("🚪 Gate OPENED temporarily - " + reason);
     addAlert("INFO", "Gate opened temporarily - " + reason, "LOW");
 
     // Update LCD to show gate status
     lcd.clear();
     lcd.setCursor(0, 0);
     lcd.print("R:" + String(ESP.getFreeHeap() / 1024) + "kb");
     lcd.setCursor(13, 0);
     lcd.print("P:" + String(passengerCount) + " T:" + String(currentBus.tapInCount));
     lcd.setCursor(0, 1);
     lcd.print("GATE OPEN");
     lcd.setCursor(0, 2);
     lcd.print(reason);
     lcd.setCursor(0, 3);
     lcd.print("Auto-close: 10s");
 }
 
 // Check for auto-close gate timer
 void checkGateAutoClose()
 {
     if (gateAutoCloseEnabled && gateOpen && (millis() - gateOpenTime > GATE_AUTO_CLOSE_TIME))
     {
         closeGate();
         gateAutoCloseEnabled = false;
         Serial.println("🚪 Gate auto-closed after 10 seconds");
         addAlert("INFO", "Gate auto-closed after timeout", "LOW");
     }
 }
 
 void checkGateControl()
 {
     // Check auto-close timer
     checkGateAutoClose();
 
     // Advanced gate control logic:
     // Gate is closed by default
     // Opens only for valid tap-in (registered user with 10+ Tk balance)
     // Opens only for valid tap-out (successful fare deduction)
     // After 5 passengers: no gate opening for tap-in
 
     // No automatic passenger-based control - gate control is NFC-driven only
 }
 
 // Data Persistence Functions
 void saveUserData()
 {
     preferences.begin("smartbus", false);
     preferences.putInt("userCount", users.size());
 
     for (int i = 0; i < users.size() && i < 50; i++)
     { // Limit to 50 users for memory
         String prefix = "user" + String(i) + "_";
         preferences.putString((prefix + "uid").c_str(), users[i].uid);
         preferences.putString((prefix + "name").c_str(), users[i].name);
         preferences.putString((prefix + "type").c_str(), users[i].userType);
         preferences.putFloat((prefix + "balance").c_str(), users[i].balance);
         preferences.putBool((prefix + "reg").c_str(), users[i].isRegistered);
         preferences.putInt((prefix + "trips").c_str(), users[i].totalTrips);
         preferences.putFloat((prefix + "spent").c_str(), users[i].totalSpent);
         preferences.putString((prefix + "phone").c_str(), users[i].phoneNumber);
         preferences.putString((prefix + "addr").c_str(), users[i].address);
         preferences.putBool((prefix + "blocked").c_str(), users[i].isBlocked);
         preferences.putString((prefix + "regdate").c_str(), users[i].registrationDate);
     }
     preferences.end();
     Serial.println("✅ User data saved to persistent storage");
 }
 
 void loadUserData()
 {
     preferences.begin("smartbus", true);
     int userCount = preferences.getInt("userCount", 0);
 
     users.clear();
     for (int i = 0; i < userCount && i < 50; i++)
     {
         String prefix = "user" + String(i) + "_";
         User user;
         user.uid = preferences.getString((prefix + "uid").c_str(), "");
         user.name = preferences.getString((prefix + "name").c_str(), "");
         user.userType = preferences.getString((prefix + "type").c_str(), "General");
         user.balance = preferences.getFloat((prefix + "balance").c_str(), 0.0);
         user.isRegistered = preferences.getBool((prefix + "reg").c_str(), false);
         user.totalTrips = preferences.getInt((prefix + "trips").c_str(), 0);
         user.totalSpent = preferences.getFloat((prefix + "spent").c_str(), 0.0);
         user.phoneNumber = preferences.getString((prefix + "phone").c_str(), "");
         user.address = preferences.getString((prefix + "addr").c_str(), "");
         user.isBlocked = preferences.getBool((prefix + "blocked").c_str(), false);
         user.registrationDate = preferences.getString((prefix + "regdate").c_str(), "");
         user.lastTapTime = 0;
 
         if (user.uid != "" && user.name != "")
         {
             users.push_back(user);
         }
     }
     preferences.end();
     Serial.println("✅ Loaded " + String(users.size()) + " users from persistent storage");
 }
 
 void clearUserData()
 {
     preferences.begin("smartbus", false);
     preferences.clear();
     preferences.end();
     Serial.println("✅ Persistent storage cleared");
 }
 
 void setup()
 {
     Serial.begin(115200);
 
     // Initialize LCD light pins and servo
     pinMode(LCD_LIGHT_WHITE, OUTPUT);
     pinMode(LCD_LIGHT_GREEN_1, OUTPUT);
     pinMode(LCD_LIGHT_GREEN_2, OUTPUT);
     pinMode(LCD_LIGHT_RED_1, OUTPUT);
     pinMode(LCD_LIGHT_RED_2, OUTPUT);
 
     // Initialize servo for gate control
     initializeServo();
 
     // Turn on white light during initialization
     setLcdLights(LIGHT_WHITE);
 
     // Initialize LCD
     lcd.init();
     lcd.backlight();
     lcd.setCursor(0, 0);
     lcd.print("ESP32 SmartBus");
     lcd.setCursor(0, 1);
     lcd.print("System Starting...");
     lcd.setCursor(0, 2);
     lcd.print("20x4 LCD Display");
     lcd.setCursor(0, 3);
     lcd.print("Please wait...");
 
     // Initialize SPI and RFID
     SPI.begin();
     rfid.PCD_Init();
 
     // Initialize sensor pins
     pinMode(TRIG_PIN, OUTPUT);
     pinMode(ECHO_PIN, INPUT);
     pinMode(IR_ENTER, INPUT_PULLUP);
     pinMode(IR_EXIT, INPUT_PULLUP);
 
     // Load persistent user data first
     loadUserData();
 
     // Initialize default data if no users loaded
     if (users.size() == 0)
     {
         initializeSystemData();
         saveUserData(); // Save initial data
     }
     else
     {
         // Initialize other system components
         initializeSystemComponents();
     }
 
     // Start WiFi Access Point
     WiFi.softAP(ssid, password);
     IPAddress IP = WiFi.softAPIP();
 
     Serial.println("WiFi AP Started");
     Serial.print("AP IP address: ");
     Serial.println(IP);
 
     // Setup web server routes
     setupWebServer();
     server.begin();
 
     // Update LCD with green lights for ready status
     setLcdLights(LIGHT_GREEN);
     lcd.clear();
     lcd.setCursor(0, 0);
     lcd.print("WiFi AP: SmartBus");
     lcd.setCursor(0, 1);
     lcd.print("IP: " + IP.toString());
     lcd.setCursor(0, 2);
     lcd.print("Password: 12345678");
     lcd.setCursor(0, 3);
     lcd.print("System Ready!");
     
     setServoAngle(SERVO_CLOSE_ANGLE);
 
     delay(5000);
     setLcdLights(LIGHT_WHITE); // Normal operation mode
 
     // Ensure gate is in correct position based on passenger count at startup
     checkGateControl();
 
     Serial.println("=== ESP32 SmartBus System Ready ===");
     Serial.println("NFC + Sensors + Web Server Active");
     Serial.println("Persistent Data Storage Active");
 }
 
 void loop()
 {
     // Memory monitoring and protection
     static unsigned long lastMemoryCheck = 0;
     if (millis() - lastMemoryCheck > 5000)
     { // Check every 5 seconds
         if (ESP.getFreeHeap() < 15000)
         { // Critical memory threshold
             Serial.println("⚠️ CRITICAL: Low memory detected: " + String(ESP.getFreeHeap()) + " bytes");
             // Force garbage collection and limit operations
             delay(100);
             if (ESP.getFreeHeap() < 10000)
             {
                 Serial.println("🚨 EMERGENCY: Memory critically low, system may become unstable");
                 addAlert("CRITICAL", "Memory critically low: " + String(ESP.getFreeHeap()) + " bytes", "CRITICAL");
             }
         }
         lastMemoryCheck = millis();
     }
 
     // Enhanced web server handling with global request limiting
     static unsigned long lastWebRequest = 0;
     static int consecutiveRequests = 0;
 
     // Global rate limiting: max 5 requests per second
     if (millis() - lastWebRequest < 200)
     {
         consecutiveRequests++;
         if (consecutiveRequests > 10)
         {
             // Skip handling requests if too many consecutive requests
             delay(50); // Small delay to prevent server overload
             consecutiveRequests = 0;
         }
     }
     else
     {
         consecutiveRequests = 0;
     }
 
     // Only handle web requests if we have sufficient memory
     if (ESP.getFreeHeap() > 12000)
     {
         server.handleClient();
         lastWebRequest = millis();
     }
     else
     {
         // Skip web handling if memory is low
         delay(10);
     }
 
     unsigned long currentTime = millis();
 
     // Handle sensors every 150ms (faster response)
     if (currentTime - lastSensorRead >= SENSOR_READ_INTERVAL)
     {
         handleSensors();
         lastSensorRead = currentTime;
     }
 
     // Handle NFC reading
     handleNFC();
 
     // Update LCD display every 1.5 seconds
     if (currentTime - lastLcdUpdate >= LCD_UPDATE_INTERVAL)
     {
         updateLcdDisplay();
         lastLcdUpdate = currentTime;
     }
 
     // Update analytics every 5 seconds
     if (currentTime - lastAnalyticsUpdate >= ANALYTICS_UPDATE_INTERVAL)
     {
         updateSystemStats();
         checkSpeedViolations();
         lastAnalyticsUpdate = currentTime;
 
         // Debug output for web server every 5 seconds
         static unsigned long lastWebDebug = 0;
         if (currentTime - lastWebDebug >= 10000)
         { // Every 10 seconds
             Serial.println("📊 System Status:");
             Serial.println("   WiFi Clients: " + String(WiFi.softAPgetStationNum()));
             Serial.println("   Web Requests: " + String(stats.webRequests));
             Serial.println("   Free Memory: " + String(ESP.getFreeHeap()) + " bytes");
             Serial.println("   Distance: " + String(currentDistanceCm) + " cm");
             Serial.println("   Passengers: " + String(passengerCount));
             Serial.println("   Active Trips: " + String(activeTrips.size()));
             Serial.println("   Speed: " + String(currentBus.currentSpeedKmph) + " km/h");
             lastWebDebug = currentTime;
         }
     }
 
     // Check alerts every second
     if (currentTime - lastAlertCheck >= ALERT_CHECK_INTERVAL)
     {
         cleanupOldAlerts();
         continuousLedBuzzerMonitoring(); // Enhanced: Continuous monitoring
         lastAlertCheck = currentTime;
 
         // Monitor WiFi AP status
         static int lastClientCount = -1;
         int currentClientCount = WiFi.softAPgetStationNum();
         if (currentClientCount != lastClientCount)
         {
             Serial.println("📶 WiFi clients changed: " + String(lastClientCount) + " → " + String(currentClientCount));
             lastClientCount = currentClientCount;
         }
     }
 
     // Perform comprehensive health check every 10 seconds
     static unsigned long lastHealthCheck = 0;
     if (currentTime - lastHealthCheck >= HEALTH_CHECK_INTERVAL)
     {
         performSystemHealthCheck();
         lastHealthCheck = currentTime;
     }
 
     // Check NFC registration timeout
     if (nfcRegistrationMode && currentTime > nfcRegistrationTimeout)
     {
         // Timeout - disable registration mode
         nfcRegistrationMode = false;
         lastReadUID = ""; // Clear any partial read
         Serial.println("⏰ NFC registration mode timed out");
         addAlert("INFO", "NFC registration mode timed out", "LOW");
 
         // Update LCD to show timeout
         lcd.clear();
         lcd.setCursor(0, 0);
         lcd.print("R:" + String(ESP.getFreeHeap() / 1024) + "kb");
         lcd.setCursor(13, 0);
         lcd.print("P:" + String(passengerCount) + " T:" + String(currentBus.tapInCount));
         lcd.setCursor(0, 1);
         lcd.print("NFC TIMEOUT");
         lcd.setCursor(0, 2);
         lcd.print("Registration ended");
         lcd.setCursor(0, 3);
         lcd.print("Try again if needed");
 
         delay(2000);
         setLcdLights(LIGHT_WHITE);
     }
 
     // Update statistics counters
     stats.sensorReadings++;
 
     // Check gate control status
     checkGateControl();
 
     delay(25); // Reduced delay for better responsiveness
 }
 
 void initializeSystemData()
 {
     // Initialize enhanced users with comprehensive data
     users.push_back({"12345678", "Karim Rahman", "Student", 150.0, true, 0, 25, 125.50, "+8801712345678", "Dhanmondi, Dhaka", false, "2025-01-15"});
     users.push_back({"87654321", "Rahima Begum", "Senior Citizen", 300.0, true, 0, 18, 89.75, "+8801987654321", "Gulshan, Dhaka", false, "2025-02-01"});
     users.push_back({"11223344", "Ahmad Hassan", "General", 200.0, true, 0, 12, 156.25, "+8801611223344", "Uttara, Dhaka", false, "2025-02-10"});
     users.push_back({"44332211", "Fatima Khan", "Driver", 500.0, true, 0, 5, 25.00, "+8801844332211", "Mirpur, Dhaka", false, "2025-01-20"});
     users.push_back({"99887766", "Inspector Ali", "Police", 1000.0, true, 0, 2, 10.00, "+8801899887766", "Police Plaza, Dhaka", false, "2025-01-01"});
     users.push_back({"55667788", "Dr. Rashid", "Senior Citizen", 400.0, true, 0, 30, 200.15, "+8801755667788", "Bailey Road, Dhaka", false, "2024-12-15"});
 
     initializeSystemComponents();
 }
 
 void initializeSystemComponents()
 {
     // Initialize enhanced routes with detailed stop information
     routes.clear();
 
     // Route 001: University - City Center (8 stops)
     BusRoute route001;
     route001.id = "ROUTE_001";
     route001.name = "University - City Center";
     route001.totalDistance = 17.5;
     route001.farePerKm = 5.0;
     route001.color = "#2196F3";
     route001.maxSpeed = 60;
     route001.stopCount = 8;
     route001.description = "Main university route connecting campuses to downtown";
     route001.isActive = true;
     route001.totalRevenue = 2340.75;
     route001.totalTrips = 156;
 
     route001.stops.push_back({23.7465, 90.3761, "University Gate", 0.0});
     route001.stops.push_back({23.7520, 90.3820, "Medical College", 2.5});
     route001.stops.push_back({23.7580, 90.3890, "Ramna Park", 5.0});
     route001.stops.push_back({23.7630, 90.3950, "Press Club", 7.5});
     route001.stops.push_back({23.7680, 90.4010, "Shahbagh", 10.0});
     route001.stops.push_back({23.7730, 90.4070, "High Court", 12.5});
     route001.stops.push_back({23.7780, 90.4130, "Sadarghat", 15.0});
     route001.stops.push_back({23.7830, 90.4190, "City Center", 17.5});
     routes.push_back(route001);
 
     // Route 002: Airport - Old Town (8 stops)
     BusRoute route002;
     route002.id = "ROUTE_002";
     route002.name = "Airport - Old Town";
     route002.totalDistance = 17.0;
     route002.farePerKm = 5.0;
     route002.color = "#4CAF50";
     route002.maxSpeed = 70;
     route002.stopCount = 8;
     route002.description = "Express route from Hazrat Shahjalal Airport to historic Old Dhaka";
     route002.isActive = true;
     route002.totalRevenue = 1890.50;
     route002.totalTrips = 98;
 
     route002.stops.push_back({23.8432, 90.3977, "Airport Terminal", 0.0});
     route002.stops.push_back({23.8234, 90.4156, "Hotel Intercontinental", 2.4});
     route002.stops.push_back({23.7915, 90.4026, "Farmgate", 4.8});
     route002.stops.push_back({23.7615, 90.3923, "Karwan Bazar", 7.2});
     route002.stops.push_back({23.7456, 90.3845, "Bangla Motor", 9.6});
     route002.stops.push_back({23.7234, 90.4123, "Paltan", 12.0});
     route002.stops.push_back({23.7102, 90.4234, "Gulistan", 14.4});
     route002.stops.push_back({23.7056, 90.4345, "Old Town", 17.0});
     routes.push_back(route002);
 
     // Route 003: Dhanmondi - Motijheel (8 stops)
     BusRoute route003;
     route003.id = "ROUTE_003";
     route003.name = "Dhanmondi - Motijheel";
     route003.totalDistance = 14.0;
     route003.farePerKm = 5.0;
     route003.color = "#FF9800";
     route003.maxSpeed = 50;
     route003.stopCount = 8;
     route003.description = "Business district connector through residential areas";
     route003.isActive = true;
     route003.totalRevenue = 3125.25;
     route003.totalTrips = 203;
 
     route003.stops.push_back({23.7456, 90.3712, "Dhanmondi 32", 0.0});
     route003.stops.push_back({23.7456, 90.3756, "Dhanmondi 27", 2.0});
     route003.stops.push_back({23.7389, 90.3912, "Science Lab", 4.0});
     route003.stops.push_back({23.7323, 90.3834, "New Market", 6.0});
     route003.stops.push_back({23.7234, 90.3923, "Azimpur", 8.0});
     route003.stops.push_back({23.7145, 90.4012, "Lalbagh", 10.0});
     route003.stops.push_back({23.7089, 90.4123, "Chawk Bazar", 12.0});
     route003.stops.push_back({23.7023, 90.4234, "Motijheel", 14.0});
     routes.push_back(route003);
 
     // Route 004: Gulshan - Uttara (10 stops)
     BusRoute route004;
     route004.id = "ROUTE_004";
     route004.name = "Gulshan - Uttara";
     route004.totalDistance = 22.0;
     route004.farePerKm = 6.0;
     route004.color = "#9C27B0";
     route004.maxSpeed = 80;
     route004.stopCount = 10;
     route004.description = "Premium route connecting upscale neighborhoods";
     route004.isActive = true;
     route004.totalRevenue = 1560.00;
     route004.totalTrips = 65;
 
     route004.stops.push_back({23.7945, 90.4234, "Gulshan Circle 1", 0.0});
     route004.stops.push_back({23.7989, 90.4123, "Gulshan 2", 2.2});
     route004.stops.push_back({23.8034, 90.4012, "Banani", 4.4});
     route004.stops.push_back({23.8156, 90.3934, "Mohakhali", 6.6});
     route004.stops.push_back({23.8234, 90.3845, "Wireless Gate", 8.8});
     route004.stops.push_back({23.8312, 90.3756, "Airport Road", 11.0});
     route004.stops.push_back({23.8389, 90.3667, "Khilkhet", 13.2});
     route004.stops.push_back({23.8467, 90.3578, "Kuril", 15.4});
     route004.stops.push_back({23.8545, 90.3489, "Uttara Sector 7", 17.6});
     route004.stops.push_back({23.8623, 90.3400, "Uttara Center", 22.0});
     routes.push_back(route004);
 
     // Route 005: Mirpur - Farmgate (8 stops)
     BusRoute route005;
     route005.id = "ROUTE_005";
     route005.name = "Mirpur - Farmgate";
     route005.totalDistance = 12.5;
     route005.farePerKm = 4.5;
     route005.color = "#FF5722";
     route005.maxSpeed = 45;
     route005.stopCount = 8;
     route005.description = "Local route serving residential and commercial hubs";
     route005.isActive = true;
     route005.totalRevenue = 987.25;
     route005.totalTrips = 87;
 
     route005.stops.push_back({23.8045, 90.3567, "Mirpur 1", 0.0});
     route005.stops.push_back({23.8012, 90.3623, "Mirpur 2", 1.8});
     route005.stops.push_back({23.7934, 90.3712, "Mirpur 10", 3.6});
     route005.stops.push_back({23.7867, 90.3801, "Kazipara", 5.4});
     route005.stops.push_back({23.7800, 90.3890, "Shewrapara", 7.2});
     route005.stops.push_back({23.7733, 90.3978, "Agargaon", 9.0});
     route005.stops.push_back({23.7666, 90.4067, "Taltola", 10.8});
     route005.stops.push_back({23.7600, 90.4156, "Farmgate", 12.5});
     routes.push_back(route005);
 
     // Initialize enhanced current bus status
     currentBus = {
         "ROUTE_001", "BUS_001_A", 0.0, 0.0, 0, 0, "IDLE", millis(), false, false, false, 0.0, 0, "Ahmad Hassan", "DRV_001"};
 
     // Initialize system statistics
     stats = {
         0, users.size(), 0, 0.0, 0, 0, 0, 0.0, 0, "EXCELLENT"};
 
     systemStartTime = millis();
     systemInitialized = true;
 
     Serial.println("=== ENHANCED SYSTEM INITIALIZED ===");
     Serial.println("Users: " + String(users.size()));
     Serial.println("Routes: " + String(routes.size()));
     Serial.println("System Status: READY");
 
     // Add initial system ready alert
     addAlert("INFO", "Smart Bus System initialized successfully", "LOW");
 }
 
 void handleSensors()
 {
     // Read ultrasonic sensor
     currentDistanceCm = readUltrasonicDistance();
 
     // Update bus position based on distance (simulation)
     updateBusPosition();
 
     // Handle IR sensors for passenger counting
     handlePassengerCounting();
 
     // Update bus status
     currentBus.lastUpdate = millis();
 }
 
 float readUltrasonicDistance()
 {
     digitalWrite(TRIG_PIN, LOW);
     delayMicroseconds(2);
     digitalWrite(TRIG_PIN, HIGH);
     delayMicroseconds(10);
     digitalWrite(TRIG_PIN, LOW);
 
     long duration = pulseIn(ECHO_PIN, HIGH, 30000); // 30ms timeout
     if (duration == 0)
         return currentDistanceCm; // Return last valid reading
 
     return duration * 0.034 / 2; // Convert to cm
 }
 
 void updateBusPosition()
 {
     // Enhanced: Map distance sensor reading to route position
     // 1cm = 0.5km scale with 10cm blind spot compensation
     float maxDistance = 100.0; // 100cm max range
     BusRoute *route = getCurrentRoute();
     if (!route)
         return;
 
     // Constrain distance reading
     float constrainedDistance = constrain(currentDistanceCm, 10, maxDistance);
 
     // Subtract 10cm blind spot, then convert using 1cm = 0.5km scale
     float adjustedDistance = constrainedDistance - 10.0; // Remove blind spot
     float newPosition = adjustedDistance * 0.5;          // Convert to km: 1cm = 0.5km
 
     // Ensure position is not negative
     if (newPosition < 0)
     {
         newPosition = 0;
     }
 
     // Ensure position doesn't exceed route total distance
     if (newPosition > route->totalDistance)
     {
         newPosition = route->totalDistance;
     }
 
     // Calculate speed - simplified and working version
     float timeDiff = (millis() - currentBus.lastUpdate) / 1000.0; // seconds
     if (timeDiff > 0)
     {
         float positionDiff = abs(newPosition - currentBus.currentLocationKm);
         currentBus.currentSpeedKmph = (positionDiff / timeDiff) * 3.6; // Convert m/s to km/h
     }
 
     currentBus.currentLocationKm = newPosition;
 }
 
 void handlePassengerCounting()
 {
     bool ir1 = !digitalRead(IR_ENTER); // Active LOW
     bool ir2 = !digitalRead(IR_EXIT);  // Active LOW
 
     unsigned long currentTime = millis();
 
     switch (irState)
     {
     case IDLE:
         pendingEntries = 0;
         pendingExits = 0;
         if (ir1 && !ir2)
         {
             irState = IR1_TRIGGERED;
             irStateTime = currentTime;
         }
         else if (!ir1 && ir2)
         {
             irState = IR2_TRIGGERED;
             irStateTime = currentTime;
         }
         break;
 
     case IR1_TRIGGERED:
         if (ir1 && ir2)
         {
             // Person entering - both sensors triggered
             irState = BOTH_TRIGGERED;
             irStateTime = currentTime;
         }
         else if (!ir1 && !ir2)
         {
             irState = IDLE; // Reset if no sensors triggered
         }
         else if (currentTime - irStateTime > IR_TIMEOUT)
         {
             irState = IDLE; // Timeout
         }
         break;
 
     case IR2_TRIGGERED:
         if (ir1 && ir2)
         {
             // Person exiting - both sensors triggered
             irState = BOTH_TRIGGERED;
             irStateTime = currentTime;
         }
         else if (!ir1 && !ir2)
         {
             irState = IDLE; // Reset if no sensors triggered
         }
         else if (currentTime - irStateTime > IR_TIMEOUT)
         {
             irState = IDLE; // Timeout
         }
         break;
 
     case BOTH_TRIGGERED:
         if (!ir1 && ir2)
         {
             // Completed entry sequence: IR1 -> BOTH -> IR2
             irState = ENTRY_CONFIRMED;
             irStateTime = currentTime;
         }
         else if (ir1 && !ir2)
         {
             // Completed exit sequence: IR2 -> BOTH -> IR1
             irState = EXIT_CONFIRMED;
             irStateTime = currentTime;
         }
         else if (currentTime - irStateTime > IR_TIMEOUT)
         {
             irState = IDLE; // Timeout
         }
         break;
 
     case ENTRY_CONFIRMED:
         passengerCount++;
         currentBus.irPassengerCount = passengerCount;
         pendingEntries++;
 
         // Close gate automatically when passenger enters
         if (gateOpen && gateAutoCloseEnabled)
         {
             // closeGate();
             setServoAngle(SERVO_CLOSE_ANGLE);
             gateAutoCloseEnabled = false;
             Serial.println("🚪 Gate auto-closed - passenger entered");
             addAlert("INFO", "Gate closed - passenger entered", "LOW");
         }
 
         // Visual and audio feedback for entry
         setLcdLights(LIGHT_BOARDING); // Enhanced: Special boarding lights
         delay(500);
         setLcdLights(LIGHT_WHITE); // Return to normal
 
         Serial.println("✅ Passenger ENTERED. Count: " + String(passengerCount));
         addAlert("INFO", "Passenger entered bus", "LOW");
 
         // Enhanced: Check for passenger overflow warning
         if (passengerCount > MAX_PASSENGER_LIMIT)
         {
             setLcdLights(LIGHT_PASSENGER_WARNING);
             addAlert("WARNING", "Passenger limit exceeded: " + String(passengerCount) + " passengers", "HIGH");
             delay(1000);
             setLcdLights(LIGHT_WHITE);
         }
 
         // Flash LCD for entry
         lcd.clear();
         lcd.setCursor(0, 0);
         lcd.print("R:" + String(ESP.getFreeHeap() / 1024) + "kb");
         lcd.setCursor(13, 0);
         lcd.print("P:" + String(passengerCount) + " T:" + String(currentBus.tapInCount));
         lcd.setCursor(0, 1);
         lcd.print("PASSENGER ENTERED");
         lcd.setCursor(0, 2);
         lcd.print("Current Count: " + String(passengerCount));
         lcd.setCursor(0, 3);
         lcd.print("Speed: " + String(currentBus.currentSpeedKmph, 1) + " km/h");
 
         irState = IDLE;
         break;
 
     case EXIT_CONFIRMED:
         if (passengerCount > 0)
         {
             passengerCount--;
             currentBus.irPassengerCount = passengerCount;
             pendingExits++;
 
             // Close gate automatically when passenger exits
             if (gateOpen && gateAutoCloseEnabled)
             {
                 closeGate();
                 gateAutoCloseEnabled = false;
                 Serial.println("🚪 Gate auto-closed - passenger exited");
                 addAlert("INFO", "Gate closed - passenger exited", "LOW");
             }
 
             // Visual and audio feedback for exit
             setLcdLights(LIGHT_BOARDING); // Enhanced: Special boarding lights
             delay(500);
             setLcdLights(LIGHT_WHITE); // Return to normal
 
             Serial.println("✅ Passenger EXITED. Count: " + String(passengerCount));
             addAlert("INFO", "Passenger exited bus", "LOW");
 
             // Flash LCD for exit
             lcd.clear();
             lcd.setCursor(0, 0);
             lcd.print("R:" + String(ESP.getFreeHeap() / 1024) + "kb");
             lcd.setCursor(13, 0);
             lcd.print("P:" + String(passengerCount) + " T:" + String(currentBus.tapInCount));
             lcd.setCursor(0, 1);
             lcd.print("PASSENGER EXITED");
             lcd.setCursor(0, 2);
             lcd.print("Current Count: " + String(passengerCount));
             lcd.setCursor(0, 3);
             lcd.print("Speed: " + String(currentBus.currentSpeedKmph, 1) + " km/h");
         }
         else
         {
             // Alert for impossible exit with red lights
             setLcdLights(LIGHT_RED);
             delay(1000);
             setLcdLights(LIGHT_WHITE);
             addAlert("WARNING", "Exit detected but no passengers counted", "MEDIUM");
         }
 
         irState = IDLE;
         break;
     }
 
     // Check for passenger count vs tap-in mismatch
     if (abs(passengerCount - currentBus.tapInCount) > 3)
     {
         static unsigned long lastMismatchAlert = 0;
         if (currentTime - lastMismatchAlert > 30000) // Alert every 30 seconds max
         {
             addAlert("WARNING", "Passenger count mismatch: IR=" + String(passengerCount) + " Taps=" + String(currentBus.tapInCount), "HIGH");
             lastMismatchAlert = currentTime;
         }
     }
 }
 
 void handleNFC()
 {
     if (millis() - lastNfcRead < NFC_DEBOUNCE_TIME)
         return;
 
     if (!rfid.PICC_IsNewCardPresent() || !rfid.PICC_ReadCardSerial())
     {
         return;
     }
 
     // Read UID
     String uid = "";
     for (byte i = 0; i < rfid.uid.size; i++)
     {
         uid += String(rfid.uid.uidByte[i] < 0x10 ? "0" : "");
         uid += String(rfid.uid.uidByte[i], HEX);
     }
     uid.toUpperCase();
 
     lastNfcRead = millis();
 
     // Check if we're in NFC registration mode
     if (nfcRegistrationMode)
     {
         handleNFCRegistration(uid);
     }
     else
     {
         // Normal NFC tap processing
         processNfcTap(uid);
     }
 
     // Halt PICC
     rfid.PICC_HaltA();
     rfid.PCD_StopCrypto1();
 }
 
 void handleNFCRegistration(String uid)
 {
     Serial.println("📱 NFC Registration Mode - Card detected: " + uid);
 
     // Check if this UID is already registered
     User *existingUser = findUser(uid);
     if (existingUser)
     {
         // Red lights and warning sound for already registered card
         setLcdLights(LIGHT_RED);
 
         lcd.clear();
         lcd.setCursor(0, 0);
         lcd.print("R:" + String(ESP.getFreeHeap() / 1024) + "kb");
         lcd.setCursor(13, 0);
         lcd.print("P:" + String(passengerCount) + " T:" + String(currentBus.tapInCount));
         lcd.setCursor(0, 1);
         lcd.print("CARD REGISTERED!");
         lcd.setCursor(0, 2);
         lcd.print("User: " + existingUser->name.substring(0, 14));
         lcd.setCursor(0, 3);
         lcd.print("Try another card");
 
         delay(3000);
         setLcdLights(LIGHT_WHITE);
 
         // Keep registration mode active, set lastReadUID to indicate error
         lastReadUID = "ERROR_ALREADY_REGISTERED:" + existingUser->name;
         Serial.println("❌ Card already registered to: " + existingUser->name);
         addAlert("WARNING", "Registration attempt with already registered card: " + existingUser->name, "MEDIUM");
 
         // IMPORTANT: Don't disable registration mode here, let the timeout handle it
         // This allows the web interface to read the error status
         return;
     }
 
     // Card is not registered - accept it for registration
     lastReadUID = uid;
 
     // IMPORTANT: Disable registration mode immediately after successful read
     // This triggers the web interface to detect the change and process the UID
     nfcRegistrationMode = false;
 
     // Green lights and success sound for valid unregistered card
     setLcdLights(LIGHT_GREEN);
 
     lcd.clear();
     lcd.setCursor(0, 0);
     lcd.print("R:" + String(ESP.getFreeHeap() / 1024) + "kb");
     lcd.setCursor(13, 0);
     lcd.print("P:" + String(passengerCount) + " T:" + String(currentBus.tapInCount));
     lcd.setCursor(0, 1);
     lcd.print("NFC UID CAPTURED");
     lcd.setCursor(0, 2);
     lcd.print("UID: " + uid.substring(0, 16));
     lcd.setCursor(0, 3);
     lcd.print("Complete registration");
 
     delay(3000);
     setLcdLights(LIGHT_WHITE);
 
     Serial.println("✅ Unregistered card accepted for registration: " + uid);
     Serial.println("📡 Registration mode disabled, web should auto-fill UID now");
     addAlert("INFO", "NFC card captured for registration: " + uid, "LOW");
 }
 
 void processNfcTap(String uid)
 {
     Serial.println("NFC Card detected: " + uid);
 
     // Find user
     User *user = findUser(uid);
     if (!user)
     {
         // Red lights and error sound for unknown card
         setLcdLights(LIGHT_RED);
 
         lcd.clear();
         lcd.setCursor(0, 0);
         lcd.print("R:" + String(ESP.getFreeHeap() / 1024) + "kb");
         lcd.setCursor(13, 0);
         lcd.print("P:" + String(passengerCount) + " T:" + String(currentBus.tapInCount));
         lcd.setCursor(0, 1);
         lcd.print("CARD NOT FOUND");
         lcd.setCursor(0, 2);
         lcd.print("UID: " + uid.substring(0, 16));
         lcd.setCursor(0, 3);
         lcd.print("Register via web");
 
         delay(5000);
         setLcdLights(LIGHT_WHITE); // Return to normal
         Serial.println("Unknown card: " + uid);
         return;
     }
 
     // Check if user has active trip
     Trip *activeTrip = findActiveTrip(uid);
 
     if (activeTrip)
     {
         // End trip
         endTrip(activeTrip, user);
     }
     else
     {
         // Start trip
         startTrip(user);
     }
 
     // Save user data after each transaction
     saveUserData();
 }
 
 User *findUser(String uid)
 {
     for (auto &user : users)
     {
         if (user.uid == uid)
         {
             return &user;
         }
     }
     return nullptr;
 }
 
 Trip *findActiveTrip(String uid)
 {
     for (auto &trip : activeTrips)
     {
         if (trip.uid == uid && trip.isActive)
         {
             return &trip;
         }
     }
     return nullptr;
 }
 
 void startTrip(User *user)
 {
     // Check if bus is at capacity first
     if (passengerCount >= MAX_PASSENGER_LIMIT)
     {
         // Red lights and show "Passenger Full" message
         setLcdLights(LIGHT_RED);
 
         lcd.clear();
         lcd.setCursor(0, 0);
         lcd.print("R:" + String(ESP.getFreeHeap() / 1024) + "kb");
         lcd.setCursor(13, 0);
         lcd.print("P:" + String(passengerCount) + " T:" + String(currentBus.tapInCount));
         lcd.setCursor(0, 1);
         lcd.print("PASSENGER FULL");
         lcd.setCursor(0, 2);
         lcd.print("Capacity: " + String(MAX_PASSENGER_LIMIT));
         lcd.setCursor(0, 3);
         lcd.print("Wait for exit");
 
         delay(3000);
         setLcdLights(LIGHT_WHITE);
         Serial.println("❌ Bus at capacity - no boarding allowed");
         addAlert("WARNING", "Tap-in denied - Bus at capacity: " + user->name, "HIGH");
         return; // NO TRIP STARTS, NO WEB POPUP
     }
 
     // Check minimum balance for entry (10 Tk)
     if (user->balance < MIN_BALANCE_FOR_ENTRY)
     {
         // Red lights and warning sound for low balance
         setLcdLights(LIGHT_RED);
 
         lcd.clear();
         lcd.setCursor(0, 0);
         lcd.print("R:" + String(ESP.getFreeHeap() / 1024) + "kb");
         lcd.setCursor(13, 0);
         lcd.print("P:" + String(passengerCount) + " T:" + String(currentBus.tapInCount));
         lcd.setCursor(0, 1);
         lcd.print("LOW BALANCE");
         lcd.setCursor(0, 2);
         lcd.print("Current: " + String(user->balance, 1) + " Tk");
         lcd.setCursor(0, 3);
         lcd.print("Required: " + String(MIN_BALANCE_FOR_ENTRY, 0) + " Tk");
 
         delay(3000);
         setLcdLights(LIGHT_WHITE);
         Serial.println("❌ Insufficient balance for " + user->name + ": " + String(user->balance));
         addAlert("WARNING", "Trip denied - insufficient balance: " + user->name, "MEDIUM");
         return; // NO GATE OPENING
     }
 
     // Check if user is blocked
     if (user->isBlocked)
     {
         setLcdLights(LIGHT_RED);
         // playBuzzer(4); // Error sound
 
         lcd.clear();
         lcd.setCursor(0, 0);
         lcd.print("ACCOUNT BLOCKED");
         lcd.setCursor(0, 1);
         lcd.print("Contact Admin");
 
         delay(2000);
         setLcdLights(LIGHT_WHITE);
         addAlert("ERROR", "Blocked user attempted trip: " + user->name, "HIGH");
         return;
     }
 
     // Create new enhanced trip
     Trip newTrip;
     newTrip.uid = user->uid;
     newTrip.userName = user->name;
     newTrip.routeId = currentBus.routeId;
     newTrip.busId = currentBus.busId;
     newTrip.startLocationKm = currentBus.currentLocationKm;
     newTrip.endLocationKm = 0;
     newTrip.startTime = millis();
     newTrip.endTime = 0;
     newTrip.fare = 0;
     newTrip.distance = 0;
     newTrip.isActive = true;
     newTrip.tripStatus = "ACTIVE";
     newTrip.rating = 0;
     newTrip.feedback = "";
 
     activeTrips.push_back(newTrip);
     currentBus.tapInCount++;
 
     // Deduct minimum fare as deposit
     user->balance -= MIN_BALANCE;
     user->lastTapTime = millis();
 
     // Update route statistics
     BusRoute *route = getCurrentRoute();
     if (route)
     {
         route->totalTrips++;
     }
 
     // Green lights and success sound for trip start
     setLcdLights(LIGHT_GREEN);
 
     lcd.clear();
     lcd.setCursor(0, 0);
     lcd.print("R:" + String(ESP.getFreeHeap() / 1024) + "kb");
     lcd.setCursor(13, 0);
     lcd.print("P:" + String(passengerCount) + " T:" + String(currentBus.tapInCount));
     lcd.setCursor(0, 1);
     lcd.print("TRIP STARTED");
     lcd.setCursor(0, 2);
     lcd.print("User: " + user->name.substring(0, 14));
     lcd.setCursor(0, 3);
     lcd.print("Balance: " + String(user->balance, 1) + " Tk");
 
     Serial.println("✅ Trip started for " + user->name + " on " + currentBus.routeId);
     Serial.println("   Start location: " + String(currentBus.currentLocationKm) + " km");
     Serial.println("   Remaining balance: " + String(user->balance) + " Tk");
 
     addAlert("INFO", "Trip started: " + user->name + " on " + currentBus.routeId, "LOW");
 
     // Open gate temporarily for validated user
     openGateTemporarily("Valid tap-in: " + user->name);
 
     // Add trip event for web popup
     addTripEvent("trip_start", user);
 
     delay(2000);               // Show trip started message
     setLcdLights(LIGHT_WHITE); // Return to normal
 }
 
 void endTrip(Trip *trip, User *user)
 {
     // Calculate trip details
     float distance = abs(currentBus.currentLocationKm - trip->startLocationKm);
     if (distance < 0.5)
         distance = 0.5; // Minimum distance charge
 
     unsigned long tripDuration = millis() - trip->startTime;
     float tripDurationMinutes = tripDuration / 60000.0;
 
     // Calculate dynamic fare
     float totalFare = calculateDynamicFare(distance, user->userType, user->totalTrips);
 
     // Handle refund or additional charge
     float refund = MIN_BALANCE - totalFare;
     if (refund > 0)
     {
         user->balance += refund;
     }
     else
     {
         user->balance -= abs(refund);
         if (user->balance < 0)
         {
             addAlert("WARNING", "Negative balance for " + user->name + ": " + String(user->balance), "MEDIUM");
             user->balance = 0; // Prevent negative balance
         }
     }
 
     // Update trip record
     trip->endLocationKm = currentBus.currentLocationKm;
     trip->endTime = millis();
     trip->fare = totalFare;
     trip->distance = distance;
     trip->isActive = false;
     trip->tripStatus = "COMPLETED";
 
     // Update user statistics
     user->totalTrips++;
     user->totalSpent += totalFare;
 
     // Update route revenue
     BusRoute *route = getCurrentRoute();
     if (route)
     {
         route->totalRevenue += totalFare;
     }
 
     // Move trip to completed trips
     completedTrips.push_back(*trip);
 
     // Remove from active trips
     for (auto it = activeTrips.begin(); it != activeTrips.end(); ++it)
     {
         if (it->uid == trip->uid)
         {
             activeTrips.erase(it);
             break;
         }
     }
 
     currentBus.tapInCount--;
     if (currentBus.tapInCount < 0)
         currentBus.tapInCount = 0;
 
     // Calculate discount percentage for display
     float discountPercent = 0;
     if (user->userType == "Student")
         discountPercent = STUDENT_DISCOUNT * 100;
     else if (user->userType == "Senior Citizen")
         discountPercent = SENIOR_DISCOUNT * 100;
 
     // Green lights and success sound for trip end
     setLcdLights(LIGHT_GREEN);
 
     // Gate control: Open gate temporarily for successful tap-out
     openGateTemporarily("Successful tap-out: " + user->name);
 
     // Display comprehensive trip summary
     lcd.clear();
     lcd.setCursor(0, 0);
     lcd.print("R:" + String(ESP.getFreeHeap() / 1024) + "kb");
     lcd.setCursor(13, 0);
     lcd.print("P:" + String(passengerCount) + " T:" + String(currentBus.tapInCount));
     lcd.setCursor(0, 1);
     lcd.print("TRIP ENDED");
     lcd.setCursor(0, 2);
     lcd.print("Fare: " + String(totalFare, 1) + " Tk");
     lcd.setCursor(0, 3);
     lcd.print("Distance: " + String(distance, 1) + " km");
 
     delay(2000);
 
     if (discountPercent > 0)
     {
         lcd.clear();
         lcd.setCursor(0, 0);
         lcd.print("R:" + String(ESP.getFreeHeap() / 1024) + "kb");
         lcd.setCursor(13, 0);
         lcd.print("P:" + String(passengerCount) + " T:" + String(currentBus.tapInCount));
         lcd.setCursor(0, 1);
         lcd.print("Discount: " + String(discountPercent, 0) + "%");
         lcd.setCursor(0, 2);
         lcd.print("Saved: " + String((distance * BASE_RATE_PER_KM - totalFare), 1) + " Tk");
         lcd.setCursor(0, 3);
         lcd.print("Balance: " + String(user->balance, 1) + " Tk");
         delay(2000);
     }
 
     setLcdLights(LIGHT_WHITE); // Return to normal
 
     Serial.println("✅ Trip completed for " + user->name);
     Serial.println("   Distance: " + String(distance, 2) + " km");
     Serial.println("   Duration: " + String(tripDurationMinutes, 1) + " minutes");
     Serial.println("   Fare: " + String(totalFare, 2) + " Tk");
     Serial.println("   User Type: " + user->userType);
     Serial.println("   Remaining balance: " + String(user->balance, 2) + " Tk");
 
     addAlert("INFO", "Trip completed: " + user->name + " - " + String(totalFare, 2) + " Tk", "LOW");
 
     // Add trip event for web popup
     addTripEvent("trip_end", user, trip, totalFare, distance, (int)tripDurationMinutes, discountPercent);
 }
 
 BusRoute *getCurrentRoute()
 {
     for (auto &route : routes)
     {
         if (route.id == currentBus.routeId)
         {
             return &route;
         }
     }
     return nullptr;
 }
 
 String getNextStopName(float currentPosition)
 {
     BusRoute *route = getCurrentRoute();
     if (!route || route->stops.size() == 0)
     {
         return "No Route Selected";
     }
 
     // Find the next stop ahead of current position
     for (const auto &stop : route->stops)
     {
         if (stop.km > currentPosition)
         {
             float distanceToStop = stop.km - currentPosition;
             if (distanceToStop <= 2.0) // Within 2km of the stop
             {
                 return "Next: " + stop.stopName + " (" + String(distanceToStop, 1) + "km)";
             }
             else
             {
                 return "Next: " + stop.stopName;
             }
         }
     }
 
     // If no stop ahead, we're at or past the final stop
     if (route->stops.size() > 0)
     {
         return "Final: " + route->stops.back().stopName;
     }
 
     return "No Stops Found";
 }
 
 String getCurrentStopInfo(float currentPosition)
 {
     BusRoute *route = getCurrentRoute();
     if (!route || route->stops.size() == 0)
     {
         return "Position: " + String(currentPosition, 1) + "km";
     }
 
     // Find the nearest stop to current position
     float minDistance = 999.0;
     String nearestStop = "";
 
     for (const auto &stop : route->stops)
     {
         float distance = abs(stop.km - currentPosition);
         if (distance < minDistance)
         {
             minDistance = distance;
             nearestStop = stop.stopName;
         }
     }
 
     if (minDistance <= 0.5) // Within 500m of a stop
     {
         return "At: " + nearestStop;
     }
     else
     {
         return "Near: " + nearestStop + " (" + String(minDistance, 1) + "km)";
     }
 }
 
 // Enhanced Helper Functions
 void addAlert(String type, String message, String severity)
 {
     Alert newAlert;
     newAlert.type = type;
     newAlert.message = message;
     newAlert.timestamp = millis();
     newAlert.isActive = true;
     newAlert.severity = severity;
 
     systemAlerts.push_back(newAlert);
 
     // Keep only last 20 alerts to save memory
     if (systemAlerts.size() > 20)
     {
         systemAlerts.erase(systemAlerts.begin());
     }
 
     Serial.println("[" + type + "] " + message);
 }
 
 void updateSystemStats()
 {
     stats.uptime = millis() - systemStartTime;
     stats.totalUsers = users.size();
     stats.activeTrips = activeTrips.size();
 
     // Calculate total revenue
     stats.totalRevenue = 0;
     for (const auto &route : routes)
     {
         stats.totalRevenue += route.totalRevenue;
     }
 
     // Calculate average speed
     stats.averageSpeed = currentBus.currentSpeedKmph;
 
     // Update system health based on various factors
     if (currentBus.emergencyMode)
     {
         stats.systemHealth = "CRITICAL";
     }
     else if (currentBus.speedWarning || currentBus.illegalStopWarning)
     {
         stats.systemHealth = "WARNING";
     }
     else if (stats.activeTrips > 0)
     {
         stats.systemHealth = "GOOD";
     }
     else
     {
         stats.systemHealth = "EXCELLENT";
     }
 
     // Ensure bus passenger count matches global count
     currentBus.irPassengerCount = passengerCount;
 }
 
 bool isPeakHour()
 {
     // Simulate peak hours: 7-9 AM and 5-7 PM
     unsigned long currentTime = millis() / 1000 / 60; // minutes since start
     int hourOfDay = (currentTime / 60) % 24;          // simulate hour of day
     return (hourOfDay >= 7 && hourOfDay <= 9) || (hourOfDay >= 17 && hourOfDay <= 19);
 }
 
 float calculateDynamicFare(float distance, String userType, int totalTrips)
 {
     float baseFare = distance * BASE_RATE_PER_KM;
 
     // Apply peak hour pricing
     if (isPeakHour())
     {
         baseFare *= PEAK_HOUR_MULTIPLIER;
     }
 
     // Apply user type discounts
     float discount = 0;
     if (userType == "Student")
     {
         discount = STUDENT_DISCOUNT;
     }
     else if (userType == "Senior Citizen")
     {
         discount = SENIOR_DISCOUNT;
     }
 
     // Apply loyalty discount for frequent users
     if (totalTrips > 20)
     {
         discount += LOYALTY_DISCOUNT;
         if (discount > 0.7)
             discount = 0.7; // Max 70% discount
     }
 
     return baseFare * (1 - discount);
 }
 
 void checkSpeedViolations()
 {
     // Enhanced: Check for speed warning at 20 km/h
     if (currentBus.currentSpeedKmph > SPEED_WARNING_THRESHOLD)
     {
         static unsigned long lastSpeedWarningTime = 0;
         if (millis() - lastSpeedWarningTime > 5000) // Warning every 5 seconds
         {
             setLcdLights(LIGHT_SPEED_WARNING); // Enhanced: Special speed warning lights
             addAlert("WARNING", "Speed warning: " + String(currentBus.currentSpeedKmph, 1) + " km/h (limit: " + String(SPEED_WARNING_THRESHOLD, 0) + " km/h)", "HIGH");
             lastSpeedWarningTime = millis();
         }
         currentBus.speedWarning = true;
     }
     else
     {
         currentBus.speedWarning = false;
     }
 
     // Check for maximum speed limit violation
     if (currentBus.currentSpeedKmph > MAX_SPEED_LIMIT)
     {
         if (!currentBus.speedWarning)
         {
             currentBus.speedWarning = true;
             setLcdLights(LIGHT_ALERT); // Flash red lights for speed limit violation
             addAlert("CRITICAL", "Speed limit exceeded: " + String(currentBus.currentSpeedKmph) + " km/h", "CRITICAL");
         }
     }
 
     // Enhanced: Check for passenger count warnings
     static unsigned long lastPassengerWarningTime = 0;
     if (passengerCount > MAX_PASSENGER_LIMIT && millis() - lastPassengerWarningTime > 10000) // Warning every 10 seconds
     {
         setLcdLights(LIGHT_PASSENGER_WARNING);
         addAlert("WARNING", "Passenger capacity exceeded: " + String(passengerCount) + "/" + String(MAX_PASSENGER_LIMIT), "MEDIUM");
         lastPassengerWarningTime = millis();
     }
 
     // Check for emergency stop condition
     if (currentDistanceCm <= EMERGENCY_STOP_THRESHOLD && currentBus.currentSpeedKmph > 10)
     {
         if (!currentBus.emergencyMode)
         {
             currentBus.emergencyMode = true;
             setLcdLights(LIGHT_ALERT); // Flash red lights
             addAlert("EMERGENCY", "Emergency stop required - obstacle detected", "CRITICAL");
         }
     }
     else
     {
         currentBus.emergencyMode = false;
     }
 }
 
 void performSystemHealthCheck()
 {
     // Check various system components
     bool allSensorsWorking = true;
 
     // Check NFC sensor
     if (millis() - lastNfcRead > 300000)
     { // No NFC activity for 5 minutes
         addAlert("WARNING", "NFC sensor inactive for extended period", "MEDIUM");
     }
 
     // Check distance sensor
     if (currentDistanceCm <= 0 || currentDistanceCm > 400)
     {
         allSensorsWorking = false;
         addAlert("ERROR", "Distance sensor reading invalid", "HIGH");
     }
 
     // Check memory usage
     if (ESP.getFreeHeap() < 10000)
     { // Less than 10KB free
         addAlert("WARNING", "Low memory: " + String(ESP.getFreeHeap()) + " bytes", "HIGH");
     }
 
     // Update system health
     updateSystemStats();
 }
 
 String formatTime(unsigned long timestamp)
 {
     unsigned long seconds = timestamp / 1000;
     unsigned long minutes = seconds / 60;
     unsigned long hours = minutes / 60;
 
     return String(hours % 24) + ":" +
            String((minutes % 60) < 10 ? "0" : "") + String(minutes % 60) + ":" +
            String((seconds % 60) < 10 ? "0" : "") + String(seconds % 60);
 }
 
 void cleanupOldAlerts()
 {
     // Remove alerts older than 1 hour
     unsigned long cutoffTime = millis() - 3600000; // 1 hour
 
     for (auto it = systemAlerts.begin(); it != systemAlerts.end();)
     {
         if (it->timestamp < cutoffTime)
         {
             it = systemAlerts.erase(it);
         }
         else
         {
             ++it;
         }
     }
 }
 
 void updateLcdDisplay()
 {
     // Enhanced LCD display for 20x4 with 8 different modes
     // Special display mode when in NFC registration
     if (nfcRegistrationMode)
     {
         lcd.clear();
         lcd.setCursor(0, 0);
         lcd.print("R:" + String(ESP.getFreeHeap() / 1024) + "kb");
         lcd.setCursor(13, 0);
         lcd.print("P:" + String(passengerCount) + " T:" + String(currentBus.tapInCount));
         lcd.setCursor(0, 1);
         lcd.print("NFC REGISTRATION");
         lcd.setCursor(0, 2);
 
         if (lastReadUID != "")
         {
             if (lastReadUID.startsWith("ERROR_"))
             {
                 lcd.print("Card registered!");
                 lcd.setCursor(0, 3);
                 lcd.print("Try another card");
             }
             else
             {
                 lcd.print("Card read: ");
                 lcd.setCursor(0, 3);
                 lcd.print(lastReadUID.substring(0, 20));
             }
         }
         else
         {
             lcd.print("Tap card to read");
             lcd.setCursor(0, 3);
             long timeLeft = (nfcRegistrationTimeout - millis()) / 1000;
             if (timeLeft < 0)
                 timeLeft = 0;
             lcd.print("Timeout: " + String(timeLeft) + "s");
         }
         return; // Exit early when in registration mode
     }
 
     lcdDisplayMode = (lcdDisplayMode + 1) % 8;
 
     lcd.clear();
 
     // Always show RAM usage in left upper corner
     lcd.setCursor(0, 0);
     lcd.print("R:" + String(ESP.getFreeHeap() / 1024) + "kb");
 
     // Always show passenger and tapped count in right upper corner
     lcd.setCursor(13, 0);
     lcd.print("P:" + String(passengerCount) + " T:" + String(currentBus.tapInCount));
 
     switch (lcdDisplayMode)
     {
     case 0:
         // Route and position info with next stop
         lcd.setCursor(0, 1);
         lcd.print("Route: " + currentBus.routeId.substring(6));
         lcd.setCursor(0, 2);
         lcd.print(getCurrentStopInfo(currentBus.currentLocationKm));
         lcd.setCursor(0, 3);
         lcd.print(getNextStopName(currentBus.currentLocationKm).substring(0, 20));
         break;
 
     case 1:
         // Distance sensor and next stop info
         {
             float adjustedCm = constrain(currentDistanceCm, 10, 100) - 10.0;
             lcd.setCursor(0, 1);
             lcd.print("Raw:" + String(currentDistanceCm, 0) + " Adj:" + String(adjustedCm * 0.5, 1) + "km");
             lcd.setCursor(0, 2);
             lcd.print(getNextStopName(currentBus.currentLocationKm).substring(0, 20));
             lcd.setCursor(0, 3);
             lcd.print("Speed: " + String(currentBus.currentSpeedKmph, 1) + " km/h");
         }
         break;
 
     case 2:
         // Revenue and trip statistics
         lcd.setCursor(0, 1);
         lcd.print("Revenue: " + String(stats.totalRevenue, 0) + " Tk");
         lcd.setCursor(0, 2);
         lcd.print("Completed: " + String(completedTrips.size()) + " trips");
         lcd.setCursor(0, 3);
         lcd.print("Total Users: " + String(users.size()));
         break;
 
     case 3:
         // System alerts and warnings
         lcd.setCursor(0, 1);
         if (currentBus.emergencyMode)
         {
             lcd.print("EMERGENCY MODE!");
         }
         else if (currentBus.speedWarning)
         {
             lcd.print("SPEED WARNING!");
         }
         else
         {
             lcd.print("Status: NORMAL");
         }
         lcd.setCursor(0, 2);
         lcd.print("Alerts: " + String(systemAlerts.size()));
         lcd.setCursor(0, 3);
         lcd.print("WiFi Clients: " + String(WiFi.softAPgetStationNum()));
         break;
 
     case 4:
         // Driver and bus information
         lcd.setCursor(0, 1);
         lcd.print("Driver: " + currentBus.driverName.substring(0, 13));
         lcd.setCursor(0, 2);
         lcd.print("Bus ID: " + currentBus.busId);
         lcd.setCursor(0, 3);
         lcd.print("Status: " + currentBus.status);
         break;
 
     case 5:
         // System uptime and performance
         lcd.setCursor(0, 1);
         lcd.print("Uptime: " + formatTime(stats.uptime));
         lcd.setCursor(0, 2);
         lcd.print("NFC Reads: " + String(stats.nfcReadings));
         lcd.setCursor(0, 3);
         lcd.print("Sensor Reads: " + String(stats.sensorReadings));
         break;
 
     case 6:
     {
         // Route details
         BusRoute *route = getCurrentRoute();
         if (route)
         {
             lcd.setCursor(0, 1);
             lcd.print("Route: " + route->name.substring(0, 14));
             lcd.setCursor(0, 2);
             lcd.print("Distance: " + String(route->totalDistance, 1) + " km");
             lcd.setCursor(0, 3);
             lcd.print("Fare/km: " + String(route->farePerKm, 1) + " Tk");
         }
         else
         {
             lcd.setCursor(0, 1);
             lcd.print("No Route Selected");
             lcd.setCursor(0, 2);
             lcd.print("Select route from");
             lcd.setCursor(0, 3);
             lcd.print("web interface");
         }
     }
     break;
 
     case 7:
         // Ready message with NFC prompt
         lcd.setCursor(0, 1);
         lcd.print("SmartBus Ready");
         lcd.setCursor(0, 2);
         lcd.print("Tap NFC Card to");
         lcd.setCursor(0, 3);
         lcd.print("Start/End Trip");
         break;
     }
 
     // Add warning indicators in the corners
     if (currentBus.emergencyMode)
     {
         lcd.setCursor(19, 0);
         lcd.print("!");
         lcd.setCursor(19, 3);
         lcd.print("!");
     }
     else if (currentBus.speedWarning)
     {
         lcd.setCursor(19, 0);
         lcd.print("*");
     }
 
     // Add connectivity indicator
     if (WiFi.softAPgetStationNum() > 0)
     {
         lcd.setCursor(19, 3);
         lcd.print("W");
     }
 }
 
 // Enhanced LED and Buzzer Functions Implementation
 void continuousLedBuzzerMonitoring()
 {
     static unsigned long lastMonitoringTime = 0;
     static bool passengerWarningActive = false;
     static bool speedWarningActive = false;
 
     if (millis() - lastMonitoringTime < 1000)
         return; // Check every second
     lastMonitoringTime = millis();
 
     // Enhanced: Continuous passenger overflow monitoring
     if (passengerCount > MAX_PASSENGER_LIMIT)
     {
         if (!passengerWarningActive)
         {
             passengerWarningActive = true;
             // Start blinking red lights in background
             digitalWrite(LCD_LIGHT_RED_1, !digitalRead(LCD_LIGHT_RED_1));
             digitalWrite(LCD_LIGHT_RED_2, !digitalRead(LCD_LIGHT_RED_2));
         }
         else
         {
             // Continue blinking
             digitalWrite(LCD_LIGHT_RED_1, !digitalRead(LCD_LIGHT_RED_1));
             digitalWrite(LCD_LIGHT_RED_2, !digitalRead(LCD_LIGHT_RED_2));
         }
     }
     else
     {
         if (passengerWarningActive)
         {
             passengerWarningActive = false;
             // Return to normal lighting
             setLcdLights(LIGHT_WHITE);
         }
     }
 
     // Enhanced: Continuous speed monitoring for 20 km/h warning
     if (currentBus.currentSpeedKmph > SPEED_WARNING_THRESHOLD && currentBus.currentSpeedKmph <= MAX_SPEED_LIMIT)
     {
         if (!speedWarningActive)
         {
             speedWarningActive = true;
             // Subtle white light pulsing for speed warning
             digitalWrite(LCD_LIGHT_WHITE, !digitalRead(LCD_LIGHT_WHITE));
         }
         else
         {
             // Continue pulsing white light
             digitalWrite(LCD_LIGHT_WHITE, !digitalRead(LCD_LIGHT_WHITE));
         }
     }
     else
     {
         if (speedWarningActive && currentBus.currentSpeedKmph <= SPEED_WARNING_THRESHOLD)
         {
             speedWarningActive = false;
             // Return to normal lighting
             setLcdLights(LIGHT_WHITE);
         }
     }
 
     // Enhanced: System status indication
     indicateSystemStatus();
 
     // Enhanced: Check for milestones
     static int lastMilestone = 0;
     int currentMilestone = (completedTrips.size() / 10) * 10; // Every 10 trips
     if (currentMilestone > lastMilestone && currentMilestone > 0)
     {
         lastMilestone = currentMilestone;
         celebrateSystemMilestone();
     }
 }
 
 void playDepartureWarning()
 {
     // Enhanced: Bus departure warning sequence
     setLcdLights(LIGHT_ALL);
     // playBuzzer(10); // Bus departure warning
 
     lcd.clear();
     lcd.setCursor(0, 0);
     lcd.print("R:" + String(ESP.getFreeHeap() / 1024) + "kb");
     lcd.setCursor(13, 0);
     lcd.print("P:" + String(passengerCount) + " T:" + String(currentBus.tapInCount));
     lcd.setCursor(0, 1);
     lcd.print("BUS DEPARTING");
     lcd.setCursor(0, 2);
     lcd.print("Please stand clear");
     lcd.setCursor(0, 3);
     lcd.print("Doors closing...");
 
     delay(3000);
     setLcdLights(LIGHT_WHITE);
 
     Serial.println("🚌 Bus departure warning activated");
     addAlert("INFO", "Bus departure warning activated", "LOW");
 }
 
 void celebrateSystemMilestone()
 {
     // Enhanced: Celebration for system milestones
     Serial.println("🎉 System milestone reached: " + String(completedTrips.size()) + " trips completed!");
 
     // Special celebration light sequence
     for (int i = 0; i < 3; i++)
     {
         setLcdLights(LIGHT_ALL);
         delay(300);
         setLcdLights(LIGHT_OFF);
         delay(200);
     }
 
     setLcdLights(LIGHT_WHITE);
 
     addAlert("INFO", "Milestone: " + String(completedTrips.size()) + " trips completed!", "LOW");
 }
 
 void indicateSystemStatus()
 {
     // Enhanced: Visual indication of system health
     static unsigned long lastStatusIndication = 0;
 
     if (millis() - lastStatusIndication < 5000)
         return; // Every 5 seconds
     lastStatusIndication = millis();
 
     // Brief status indication
     if (stats.systemHealth == "EXCELLENT")
     {
         // Brief green flash
         digitalWrite(LCD_LIGHT_GREEN_1, HIGH);
         delay(50);
         digitalWrite(LCD_LIGHT_GREEN_1, LOW);
     }
     else if (stats.systemHealth == "WARNING" || stats.systemHealth == "CRITICAL")
     {
         // Brief red flash
         digitalWrite(LCD_LIGHT_RED_1, HIGH);
         delay(50);
         digitalWrite(LCD_LIGHT_RED_1, LOW);
     }
 
     // WiFi connectivity indication
     if (WiFi.softAPgetStationNum() > 0)
     {
         // Brief white flash for connectivity
         digitalWrite(LCD_LIGHT_WHITE, HIGH);
         delay(30);
         digitalWrite(LCD_LIGHT_WHITE, LOW);
     }
 }
 
 void setupWebServer()
 {
     // Serve main HTML page
     server.on("/", HTTP_GET, []()
               {
         Serial.println("🌐 Request: GET /");
         String html = getMainHtmlPage();
         server.send(200, "text/html", html);
         stats.webRequests++; });
 
     // Enhanced API endpoints with logging
     server.on("/api/status", HTTP_GET, []()
               {
         Serial.println("🌐 Request: GET /api/status");
         handleApiStatus(); });
     server.on("/api/users", HTTP_GET, []()
               {
         Serial.println("🌐 Request: GET /api/users");
         handleApiUsers(); });
     server.on("/api/trips", HTTP_GET, []()
               {
         Serial.println("🌐 Request: GET /api/trips");
         handleApiTrips(); });
     server.on("/api/routes", HTTP_GET, []()
               {
         Serial.println("🌐 Request: GET /api/routes");
         handleApiRoutes(); });
     server.on("/api/bus", HTTP_GET, []()
               {
         Serial.println("🌐 Request: GET /api/bus");
         handleApiBus(); });
     server.on("/api/alerts", HTTP_GET, []()
               {
         Serial.println("🌐 Request: GET /api/alerts");
         handleApiAlerts(); });
     server.on("/api/stats", HTTP_GET, []()
               {
         Serial.println("🌐 Request: GET /api/stats");
         handleApiStats(); });
     server.on("/api/analytics", HTTP_GET, []()
               {
         Serial.println("🌐 Request: GET /api/analytics");
         handleApiAnalytics(); });
     server.on("/api/health", HTTP_GET, []()
               {
         Serial.println("🌐 Request: GET /api/health");
         handleApiHealth(); });
 
     // POST endpoints
     server.on("/api/select_route", HTTP_POST, handleApiSelectRoute);
     server.on("/api/add_balance", HTTP_POST, handleApiAddBalance);
     server.on("/api/register_user", HTTP_POST, handleApiRegisterUser);
     server.on("/api/emergency_stop", HTTP_POST, handleApiEmergencyStop);
     server.on("/api/clear_alerts", HTTP_POST, handleApiClearAlerts);
     server.on("/api/block_user", HTTP_POST, handleApiBlockUser);
     server.on("/api/rate_trip", HTTP_POST, handleApiRateTrip);
     server.on("/api/system_reset", HTTP_POST, handleApiSystemReset);
 
     // New user management endpoints
     server.on("/api/delete_user", HTTP_POST, handleApiDeleteUser);
     server.on("/api/edit_user", HTTP_POST, handleApiEditUser);
     server.on("/api/trip_events", HTTP_GET, handleApiTripEvents);
     server.on("/api/save_data", HTTP_POST, handleApiSaveData);
     server.on("/api/clear_data", HTTP_POST, handleApiClearData);
 
     // NFC Registration endpoints
     server.on("/api/start_nfc_registration", HTTP_POST, handleApiStartNFCRegistration);
     server.on("/api/get_nfc_registration_status", HTTP_GET, handleApiGetNFCRegistrationStatus);
     server.on("/api/cancel_nfc_registration", HTTP_POST, handleApiCancelNFCRegistration);
 
     // New enhanced endpoints
     server.on("/api/departure_warning", HTTP_POST, []()
               {
         Serial.println("🌐 Request: POST /api/departure_warning");
         handleApiDepartureWarning(); });
     server.on("/api/celebrate_milestone", HTTP_POST, []()
               {
         Serial.println("🌐 Request: POST /api/celebrate_milestone");
         handleApiCelebrateMilestone(); });
     server.on("/api/cancel_nfc_registration", HTTP_POST, handleApiCancelNFCRegistration);
 
     // Debug endpoint
     server.on("/api/debug", HTTP_GET, []()
               {
         Serial.println("🌐 Request: GET /api/debug");
         server.sendHeader("Access-Control-Allow-Origin", "*");
         DynamicJsonDocument doc(1024);
         doc["time"] = millis();
         doc["heap"] = ESP.getFreeHeap();
         doc["clients"] = WiFi.softAPgetStationNum();
         doc["requests"] = stats.webRequests;
         doc["distance"] = currentDistanceCm;
         doc["passengers"] = passengerCount;
         doc["nfcMode"] = nfcRegistrationMode;
         doc["systemInit"] = systemInitialized;
         doc["userCount"] = users.size();
         doc["tripsActive"] = activeTrips.size();
         doc["tripsCompleted"] = completedTrips.size();
         doc["routeCount"] = routes.size();
         
         String response;
         serializeJson(doc, response);
         server.send(200, "application/json", response);
         Serial.println("🔍 Debug info sent: " + response); });
 
     // Handle CORS for all requests
     server.onNotFound([]()
                       {
         String uri = server.uri();
         String method = (server.method() == HTTP_GET) ? "GET" : 
                        (server.method() == HTTP_POST) ? "POST" : "OTHER";
         Serial.println("🌐 Request: " + method + " " + uri + " - NOT FOUND");
         
         if (server.method() == HTTP_OPTIONS) {
             server.sendHeader("Access-Control-Allow-Origin", "*");
             server.sendHeader("Access-Control-Allow-Methods", "GET,POST,OPTIONS");
             server.sendHeader("Access-Control-Allow-Headers", "Content-Type");
             server.send(200);
         } else {
             server.sendHeader("Access-Control-Allow-Origin", "*");
             server.send(404, "application/json", "{\"error\":\"Endpoint not found\",\"uri\":\"" + uri + "\",\"method\":\"" + method + "\"}");
         } });
 
     Serial.println("✅ Enhanced web server configured with " + String(24) + " endpoints");
     Serial.println("📡 Server endpoints available:");
     Serial.println("   GET  / - Main HTML page");
     Serial.println("   GET  /api/status - System status");
     Serial.println("   GET  /api/users - User management");
     Serial.println("   GET  /api/trips - Active trips");
     Serial.println("   GET  /api/routes - Route information");
     Serial.println("   GET  /api/bus - Bus status");
     Serial.println("   GET  /api/alerts - System alerts");
     Serial.println("   GET  /api/stats - System statistics");
     Serial.println("   GET  /api/health - Health check");
     Serial.println("   POST /api/* - Various management endpoints");
 }
 
 String getMainHtmlPage()
 {
     String html = "<!DOCTYPE html>";
     html += "<html><head>";
     html += "<title>ESP32 Enhanced Smart Bus System</title>";
     html += "<meta name='viewport' content='width=device-width, initial-scale=1.0'>";
     html += "<style>";
     html += "body { font-family: Arial; margin: 20px; background: linear-gradient(135deg, #667eea 0%, #764ba2 100%); color: #333; }";
     html += ".container { max-width: 1200px; margin: 0 auto; padding: 20px; }";
     html += ".header { text-align: center; color: white; margin-bottom: 30px; }";
     html += ".header h1 { font-size: 2.5rem; margin-bottom: 10px; text-shadow: 2px 2px 4px rgba(0,0,0,0.3); }";
     html += ".card { background: white; padding: 20px; margin: 10px 0; border-radius: 15px; box-shadow: 0 8px 16px rgba(0,0,0,0.1); }";
     html += ".status-bar { background: rgba(255,255,255,0.9); padding: 15px; border-radius: 10px; margin-bottom: 20px; display: flex; justify-content: space-between; }";
     html += ".status-item { text-align: center; }";
     html += ".status-value { font-size: 1.5rem; font-weight: bold; color: #2196F3; }";
     html += ".status-label { font-size: 0.9rem; color: #666; }";
     html += ".dashboard { display: grid; grid-template-columns: repeat(auto-fit, minmax(300px, 1fr)); gap: 20px; }";
     html += ".sensor-grid { display: grid; grid-template-columns: repeat(auto-fit, minmax(150px, 1fr)); gap: 10px; }";
     html += ".sensor-item { text-align: center; padding: 15px; background: #e9ecef; border-radius: 8px; }";
     html += ".sensor-value { font-size: 24px; font-weight: bold; color: #007bff; }";
     html += ".btn { background: #007bff; color: white; border: none; padding: 10px 20px; border-radius: 5px; cursor: pointer; margin: 5px; }";
     html += ".btn:hover { background: #0056b3; }";
     html += ".btn-danger { background: #dc3545; } .btn-danger:hover { background: #c82333; }";
     html += ".btn-success { background: #28a745; } .btn-success:hover { background: #218838; }";
     html += ".btn-warning { background: #ffc107; color: #212529; } .btn-warning:hover { background: #e0a800; }";
     html += "h3 { color: #2196F3; border-bottom: 2px solid #e0e0e0; padding-bottom: 10px; margin-bottom: 15px; }";
     html += "select, input { width: 100%; padding: 8px; margin: 10px 0; border-radius: 4px; border: 1px solid #ddd; transition: all 0.3s ease; }";
     html += "input:focus { border-color: #2196F3; box-shadow: 0 0 5px rgba(33, 150, 243, 0.3); }";
     html += "input.uid-filled { background: #d4edda !important; border: 2px solid #28a745 !important; }";
     html += ".alert { padding: 10px; margin: 10px 0; border-radius: 5px; }";
     html += ".alert-warning { background: #fff3cd; border: 1px solid #ffeaa7; color: #856404; }";
     html += ".alert-danger { background: #f8d7da; border: 1px solid #f5c6cb; color: #721c24; }";
     html += ".alert-info { background: #d1ecf1; border: 1px solid #bee5eb; color: #0c5460; }";
     html += ".progress-bar { background: #e0e0e0; height: 20px; border-radius: 10px; overflow: hidden; margin: 10px 0; }";
     html += ".progress-fill { background: linear-gradient(90deg, #4CAF50, #2196F3); height: 100%; transition: width 0.5s ease; }";
     html += ".route-stop { position: absolute; top: 50%; transform: translateY(-50%); width: 8px; height: 8px; background: #fff; border: 2px solid #007bff; border-radius: 50%; cursor: pointer; }";
     html += ".route-stop.passed { background: #4CAF50; border-color: #4CAF50; }";
     html += ".route-stop.current { background: #FF9800; border-color: #FF9800; box-shadow: 0 0 10px rgba(255,152,0,0.6); }";
     html += ".route-stop.upcoming { background: #fff; border-color: #007bff; }";
     html += ".stop-name { font-size: 10px; color: #333; padding: 2px 4px; background: rgba(255,255,255,0.9); border-radius: 3px; margin: 0 2px; display: inline-block; }";
     html += ".stop-name.start { background: #4CAF50; color: white; }";
     html += ".stop-name.end { background: #2196F3; color: white; }";
     html += ".stop-name.current { background: #FF9800; color: white; font-weight: bold; }";
     html += ".user-list { list-style: none; }";
     html += ".user-item { background: #f8f9fa; padding: 10px; margin: 5px 0; border-radius: 5px; border-left: 4px solid #007bff; }";
     html += ".trip-item { background: #e8f5e8; padding: 10px; margin: 5px 0; border-radius: 5px; border-left: 4px solid #28a745; }";
     html += ".analytics-grid { display: grid; grid-template-columns: repeat(auto-fit, minmax(200px, 1fr)); gap: 15px; }";
     html += ".metric { text-align: center; padding: 15px; background: #f8f9fa; border-radius: 8px; border: 2px solid #007bff; }";
     html += ".metric-value { font-size: 2rem; font-weight: bold; color: #007bff; }";
     html += ".metric-label { color: #666; margin-top: 5px; }";
     html += "@media (max-width: 768px) { .status-bar { flex-direction: column; gap: 10px; } .dashboard { grid-template-columns: 1fr; } }";
     html += "</style></head><body>";
 
     html += "<div class='container'>";
     html += "<div class='header'>";
     html += "<h1>ESP32 Enhanced Smart Bus System</h1>";
     html += "<p>Complete IoT Bus Management Solution with Advanced Analytics</p>";
     html += "</div>";
 
     // Status Bar
     html += "<div class='status-bar'>";
     html += "<div class='status-item'><div class='status-value' id='passengerCount'>0</div><div class='status-label'>Passengers</div></div>";
     html += "<div class='status-item'><div class='status-value' id='busPosition'>0.0 km</div><div class='status-label'>Position</div></div>";
     html += "<div class='status-item'><div class='status-value' id='busSpeed'>0.0 km/h</div><div class='status-label'>Speed</div></div>";
     html += "<div class='status-item'><div class='status-value' id='activeTrips'>0</div><div class='status-label'>Active Trips</div></div>";
     html += "<div class='status-item'><div class='status-value' id='systemHealth'>OK</div><div class='status-label'>System Health</div></div>";
     html += "</div>";
 
     // Horizontal Route Progress Bar (just below status bar)
     html += "<div style='background: rgba(255,255,255,0.9); padding: 15px; border-radius: 10px; margin-bottom: 20px;'>";
     html += "<div style='display: flex; justify-content: space-between; align-items: center; margin-bottom: 10px;'>";
     html += "<span style='font-weight: bold; color: #2196F3;'>Route Progress</span>";
     html += "<span id='routeInfo' style='color: #666;'>University - City Center</span>";
     html += "</div>";
     html += "<div style='position: relative; height: 30px; background: #e0e0e0; border-radius: 15px; overflow: hidden;'>";
     html += "<div id='progressFill' style='height: 100%; background: linear-gradient(90deg, #4CAF50, #2196F3); width: 0%; transition: width 0.5s ease;'></div>";
     html += "<div id='busSlider' style='position: absolute; top: 50%; transform: translateY(-50%); width: 40px; height: 40px; background: #FF5722; border-radius: 50%; border: 3px solid white; box-shadow: 0 2px 10px rgba(0,0,0,0.3); left: 0%; transition: left 0.5s ease; display: flex; align-items: center; justify-content: center; color: white; font-weight: bold; font-size: 12px;'>BUS</div>";
     html += "<div id='routeStops' style='position: absolute; top: 0; width: 100%; height: 100%;'></div>";
     html += "</div>";
     html += "<div id='stopsList' style='display: flex; justify-content: space-between; margin-top: 8px; font-size: 11px; color: #666; overflow-x: auto; white-space: nowrap;'>";
     html += "</div>";
     html += "<div style='display: flex; justify-content: space-between; margin-top: 5px; font-size: 12px; color: #666;'>";
     html += "<span><span id='routeProgress'>0%</span> completed</span>";
     html += "<span id='nextStopInfo'>Next Stop: --</span>";
     html += "<span>Distance: <span id='totalDistance'>0</span> km</span>";
     html += "</div>";
     html += "</div>";
 
     // Main Dashboard
     html += "<div class='dashboard'>";
 
     // Bus Control Panel
     html += "<div class='card'>";
     html += "<h3>Bus Control Panel</h3>";
     html += "<label for='routeSelect'>Current Route:</label>";
     html += "<select id='routeSelect' onchange='selectRoute()'>";
     html += "<option value='ROUTE_001'>University - City Center</option>";
     html += "<option value='ROUTE_002'>Airport - Old Town</option>";
     html += "<option value='ROUTE_003'>Dhanmondi - Motijheel</option>";
     html += "<option value='ROUTE_004'>Gulshan - Uttara</option>";
     html += "<option value='ROUTE_005'>Mirpur - Farmgate</option>";
     html += "</select>";
 
     html += "<div class='sensor-grid'>";
     html += "<div class='sensor-item'><div class='sensor-value' id='distance'>0 cm</div><div>Distance Sensor</div></div>";
     html += "<div class='sensor-item'><div class='sensor-value' id='currentSpeed'>0.0 km/h</div><div>Current Speed</div></div>";
     html += "</div>";
 
     html += "<div id='busAlerts'></div>";
     html += "<button class='btn btn-danger' onclick='emergencyStop()'>Emergency Stop</button>";
     html += "<button class='btn btn-warning' onclick='clearAlerts()'>Clear Alerts</button>";
     html += "<button class='btn btn-success' onclick='departureWarning()'>Departure Warning</button>";
     html += "<button class='btn' onclick='celebrateMilestone()'>Celebrate Milestone</button>";
     html += "</div>";
 
     // Real-time Analytics Card
     html += "<div class='card'>";
     html += "<h3>Real-time Analytics</h3>";
     html += "<div class='analytics-grid'>";
     html += "<div class='metric'><div class='metric-value' id='totalRevenue'>0</div><div class='metric-label'>Total Revenue (Tk)</div></div>";
     html += "<div class='metric'><div class='metric-value' id='totalUsers'>0</div><div class='metric-label'>Registered Users</div></div>";
     html += "<div class='metric'><div class='metric-value' id='completedTrips'>0</div><div class='metric-label'>Completed Trips</div></div>";
     html += "<div class='metric'><div class='metric-value' id='systemUptime'>0</div><div class='metric-label'>System Uptime</div></div>";
     html += "</div>";
     html += "</div>";
 
     // Enhanced Users Management
     html += "<div class='card'>";
     html += "<h3>Enhanced Users Management</h3>";
     html += "<ul class='user-list' id='userList'></ul>";
     html += "<div style='margin: 15px 0;'>";
     html += "<button class='btn btn-success' onclick='showAddUserForm()'>Add New User</button>";
     html += "<button class='btn btn-warning' onclick='showEditUserForm()'>Edit User</button>";
     html += "<button class='btn btn-danger' onclick='showDeleteUserForm()'>Delete User</button>";
     html += "<button class='btn' onclick='saveAllData()'>Save All Data</button>";
     html += "<button class='btn btn-warning' onclick='clearAllData()'>Clear All Data</button>";
     html += "</div>";
 
     html += "<div id='addUserForm' style='display: none; margin-top: 15px; padding: 15px; background: #f8f9fa; border-radius: 8px;'>";
     html += "<h4>Add New User</h4>";
     html += "<input type='text' id='newUserName' placeholder='Full Name'>";
     html += "<label for='newUserUID' style='font-weight: bold; margin-top: 10px; display: block;'>NFC Card UID:</label>";
     html += "<div style='display: flex; gap: 10px; align-items: center;'>";
     html += "<input type='text' id='newUserUID' placeholder='Enter UID manually or use NFC reader below' style='flex: 1;'>";
     html += "<button class='btn btn-warning' onclick='readNFCForRegistration()' id='nfcReadBtn'>Read NFC Card</button>";
     html += "</div>";
     html += "<small style='color: #666; margin-top: 5px; display: block;'>💡 You can either type the UID manually (8-32 digits) or tap the button above and place your NFC card on the ESP32 reader</small>";
     html += "<div id='nfcStatus' style='margin: 10px 0; padding: 10px; border-radius: 5px; display: none;'></div>";
     html += "<input type='text' id='newUserPhone' placeholder='Phone Number'>";
     html += "<input type='text' id='newUserAddress' placeholder='Address'>";
     html += "<select id='newUserType'>";
     html += "<option value='General'>General User</option>";
     html += "<option value='Student'>Student (50% discount)</option>";
     html += "<option value='Senior Citizen'>Senior Citizen (35% discount)</option>";
     html += "<option value='Driver'>Driver</option>";
     html += "<option value='Police'>Police</option>";
     html += "</select>";
     html += "<input type='number' id='newUserBalance' placeholder='Initial Balance (Tk)' value='100'>";
     html += "<button class='btn btn-success' onclick='addUser()'>Register User</button>";
     html += "<button class='btn' onclick='hideAddUserForm()'>Cancel</button>";
     html += "</div>";
 
     html += "<div id='editUserForm' style='display: none; margin-top: 15px; padding: 15px; background: #fff3cd; border-radius: 8px;'>";
     html += "<h4>Edit User</h4>";
     html += "<select id='editUserSelect' onchange='loadUserForEdit()'>";
     html += "<option value=''>Select User to Edit</option>";
     html += "</select>";
     html += "<input type='text' id='editUserName' placeholder='Full Name'>";
     html += "<input type='text' id='editUserPhone' placeholder='Phone Number'>";
     html += "<input type='text' id='editUserAddress' placeholder='Address'>";
     html += "<select id='editUserType'>";
     html += "<option value='General'>General User</option>";
     html += "<option value='Student'>Student</option>";
     html += "<option value='Senior Citizen'>Senior Citizen</option>";
     html += "<option value='Driver'>Driver</option>";
     html += "<option value='Police'>Police</option>";
     html += "</select>";
     html += "<input type='number' id='editUserBalance' placeholder='Balance (Tk)' step='0.01'>";
     html += "<button class='btn btn-warning' onclick='updateUser()'>Update User</button>";
     html += "<button class='btn' onclick='hideEditUserForm()'>Cancel</button>";
     html += "</div>";
 
     html += "<div id='deleteUserForm' style='display: none; margin-top: 15px; padding: 15px; background: #f8d7da; border-radius: 8px;'>";
     html += "<h4>Delete User</h4>";
     html += "<select id='deleteUserSelect'>";
     html += "<option value=''>Select User to Delete</option>";
     html += "</select>";
     html += "<p style='color: #721c24; font-weight: bold;'>Warning: This action cannot be undone!</p>";
     html += "<button class='btn btn-danger' onclick='deleteUser()'>Delete User</button>";
     html += "<button class='btn' onclick='hideDeleteUserForm()'>Cancel</button>";
     html += "</div>";
     html += "</div>";
 
     // Active Trips & Alerts
     html += "<div class='card'>";
     html += "<h3>Active Trips & Alerts</h3>";
     html += "<div id='tripList'></div>";
     html += "<div id='alertsList'></div>";
     html += "</div>";
 
     // System Controls
     html += "<div class='card'>";
     html += "<h3>System Controls</h3>";
     html += "<button class='btn' onclick='refreshData()'>Refresh Data</button>";
     html += "<button class='btn btn-warning' onclick='systemReset()'>Reset Counters</button>";
     html += "<button class='btn' onclick='downloadAnalytics()'>Download Analytics</button>";
     html += "<div style='margin-top: 15px;'>";
     html += "<p><strong>System Status:</strong> <span id='connectionStatus'>Connected</span></p>";
     html += "<p><strong>Memory Usage:</strong> <span id='memoryUsage'>--</span></p>";
     html += "<p><strong>Last Update:</strong> <span id='lastUpdate'>--</span></p>";
     html += "</div></div>";
 
     html += "</div></div>";
 
     // Trip Popup Modal
     html += "<div id='tripModal' style='display: none; position: fixed; z-index: 1000; left: 0; top: 0; width: 100%; height: 100%; background-color: rgba(0,0,0,0.5);'>";
     html += "<div style='background-color: white; margin: 15% auto; padding: 20px; border-radius: 15px; width: 80%; max-width: 500px; text-align: center; box-shadow: 0 10px 30px rgba(0,0,0,0.3);'>";
     html += "<h2 id='modalTitle' style='color: #2196F3; margin-bottom: 20px;'>Trip Update</h2>";
     html += "<div id='modalContent' style='font-size: 16px; line-height: 1.6;'>";
     html += "<!-- Trip details will be inserted here -->";
     html += "</div>";
     html += "<button class='btn' onclick='closeTripModal()' style='margin-top: 20px; min-width: 100px;'>Close</button>";
     html += "</div></div>";
 
     // UID Popup Modal
     html += "<div id='uidModal' style='display: none; position: fixed; z-index: 1001; left: 0; top: 0; width: 100%; height: 100%; background-color: rgba(0,0,0,0.7);'>";
     html += "<div style='background-color: white; margin: 20% auto; padding: 25px; border-radius: 15px; width: 90%; max-width: 450px; text-align: center; box-shadow: 0 10px 30px rgba(0,0,0,0.4); border: 3px solid #2196F3;'>";
     html += "<h2 id='uidModalTitle' style='color: #2196F3; margin-bottom: 15px; font-size: 1.8rem;'>NFC Card UID</h2>";
     html += "<div id='uidModalContent' style='font-size: 18px; line-height: 1.8; margin: 20px 0;'>";
     html += "<!-- UID details will be inserted here -->";
     html += "</div>";
     html += "<div style='margin-top: 20px;'>";
     html += "<button class='btn btn-success' onclick='copyUIDToClipboard()' id='copyUIDBtn' style='margin-right: 10px; min-width: 120px;'>📋 Copy UID</button>";
     html += "<button class='btn' onclick='closeUIDModal()' style='min-width: 100px;'>Close</button>";
     html += "</div>";
     html += "</div></div>";
 
     // Enhanced JavaScript
     html += "<script>";
     html += "let systemData = {}; let refreshInterval; let nfcRegistrationInterval;";
 
     // NFC Registration Functions
     html += "async function readNFCForRegistration() {";
     html += "const nfcBtn = document.getElementById('nfcReadBtn');";
     html += "const nfcStatus = document.getElementById('nfcStatus');";
     html += "const uidInput = document.getElementById('newUserUID');";
     html += "try {";
     html += "console.log('🔄 Starting NFC registration...');";
     html += "nfcBtn.disabled = true;";
     html += "nfcBtn.textContent = 'Starting...';";
     html += "uidInput.value = '';"; // Clear existing UID
     html += "await fetch('/api/start_nfc_registration', {method: 'POST'});";
     html += "nfcStatus.style.display = 'block';";
     html += "nfcStatus.className = 'alert alert-info';";
     html += "nfcStatus.innerHTML = '📱 <strong>NFC Mode Active</strong><br>Please tap your NFC card on the reader now...<br><em>Timeout in 30 seconds</em>';";
     html += "nfcBtn.textContent = 'Reading...';";
     html += "console.log('📡 NFC registration mode started, polling for status...');";
     html += "nfcRegistrationInterval = setInterval(checkNFCRegistrationStatus, 1000);";
     html += "} catch (error) {";
     html += "console.error('❌ Error starting NFC registration:', error);";
     html += "nfcBtn.disabled = false;";
     html += "nfcBtn.textContent = 'Read from NFC';";
     html += "nfcStatus.style.display = 'block';";
     html += "nfcStatus.className = 'alert alert-danger';";
     html += "nfcStatus.textContent = 'Error starting NFC reader';";
     html += "}";
     html += "}";
 
     html += "async function checkNFCRegistrationStatus() {";
     html += "try {";
     html += "const response = await fetch('/api/get_nfc_registration_status');";
     html += "const status = await response.json();";
     html += "console.log('NFC Status:', status);";
     html += "const nfcBtn = document.getElementById('nfcReadBtn');";
     html += "const nfcStatus = document.getElementById('nfcStatus');";
     html += "const uidInput = document.getElementById('newUserUID');";
     html += "if (status.mode === false) {";
     html += "console.log('NFC registration mode ended');";
     html += "clearInterval(nfcRegistrationInterval);";
     html += "nfcBtn.disabled = false;";
     html += "nfcBtn.textContent = 'Read from NFC';";
     html += "if (status.uid && status.uid !== '' && !status.uid.startsWith('ERROR_')) {";
     html += "console.log('UID successfully read:', status.uid);";
     html += "uidInput.value = status.uid;";
     // Visual confirmation
     html += "uidInput.style.background = '#d4edda';";
     html += "uidInput.style.border = '2px solid #28a745';";
     html += "setTimeout(() => { uidInput.style.background = ''; uidInput.style.border = ''; }, 3000);";
     html += "nfcStatus.className = 'alert alert-success';";
     html += "nfcStatus.innerHTML = '<strong>NFC Card Read Successfully!</strong><br>UID: <code>' + status.uid + '</code><br>You can now complete the registration.';";
     html += "showUIDPopup(status.uid, 'success');";
     html += "} else if (status.uid && status.uid.startsWith('ERROR_ALREADY_REGISTERED:')) {";
     html += "const userName = status.uid.split(':')[1];";
     html += "console.log('Card already registered to:', userName);";
     html += "nfcStatus.className = 'alert alert-danger';";
     html += "nfcStatus.innerHTML = '<strong>Card Already Registered!</strong><br>This card belongs to: <strong>' + userName + '</strong><br>Please use a different card.';";
     html += "showUIDPopup('Card already registered to: ' + userName, 'error');";
     html += "} else {";
     html += "console.log('⏰ NFC reading timed out or no card detected');";
     html += "nfcStatus.className = 'alert alert-warning';";
     html += "nfcStatus.innerHTML = '<strong>NFC Reading Timed Out</strong><br>No card was detected. Please try again.';";
     html += "}";
     html += "setTimeout(() => { nfcStatus.style.display = 'none'; }, 8000);";
     html += "} else {";
     html += "console.log('Still waiting for NFC card, time remaining:', status.timeRemaining, 'ms');";
     html += "}";
     html += "} catch (error) {";
     html += "console.error('Error checking NFC status:', error);";
     html += "}";
     html += "}";
 
     html += "async function cancelNFCRegistration() {";
     html += "try {";
     html += "await fetch('/api/cancel_nfc_registration', {method: 'POST'});";
     html += "clearInterval(nfcRegistrationInterval);";
     html += "const nfcBtn = document.getElementById('nfcReadBtn');";
     html += "const nfcStatus = document.getElementById('nfcStatus');";
     html += "nfcBtn.disabled = false;";
     html += "nfcBtn.textContent = 'Read from NFC';";
     html += "nfcStatus.style.display = 'none';";
     html += "} catch (error) {";
     html += "console.error('Error cancelling NFC registration:', error);";
     html += "}";
     html += "}";
 
     // Slower refresh
     html += "function startAutoRefresh() { refreshInterval = setInterval(fetchSystemData, 3000); fetchSystemData(); }";
     html += "async function fetchSystemData() {";
     html += "console.log('Fetching system data...');";
     html += "try {";
     // Shorter timeout
     html += "const timeout = 8000;";
     html += "const fetchWithTimeout = (url) => {";
     html += "return Promise.race([";
     html += "fetch(url), ";
     html += "new Promise((_, reject) => setTimeout(() => reject(new Error('Timeout')), timeout))";
     html += "]);";
     html += "};";
     html += "const statusRes = await fetchWithTimeout('/api/status');";
     html += "console.log('Status response:', statusRes.status);";
     html += "if (!statusRes.ok) {";
     html += "if (statusRes.status === 503) throw new Error('Server overloaded - retrying later');";
     html += "throw new Error('Status API failed: ' + statusRes.status);";
     html += "}";
     // Delay between requests
     html += "await new Promise(resolve => setTimeout(resolve, 200));";
     html += "const usersRes = await fetchWithTimeout('/api/users');";
     html += "console.log('Users response:', usersRes.status);";
     html += "if (!usersRes.ok) {";
     html += "if (usersRes.status === 503) throw new Error('Server overloaded - retrying later');";
     html += "throw new Error('Users API failed: ' + usersRes.status);";
     html += "}";
     // Delay between requests
     html += "await new Promise(resolve => setTimeout(resolve, 200));";
     html += "const tripsRes = await fetchWithTimeout('/api/trips');";
     html += "await new Promise(resolve => setTimeout(resolve, 200));";
     html += "const routesRes = await fetchWithTimeout('/api/routes');";
     html += "await new Promise(resolve => setTimeout(resolve, 200));";
     html += "const busRes = await fetchWithTimeout('/api/bus');";
     html += "await new Promise(resolve => setTimeout(resolve, 200));";
     html += "const alertsRes = await fetchWithTimeout('/api/alerts');";
     html += "await new Promise(resolve => setTimeout(resolve, 200));";
     html += "const statsRes = await fetchWithTimeout('/api/stats');";
     html += "await new Promise(resolve => setTimeout(resolve, 200));";
     html += "const healthRes = await fetchWithTimeout('/api/health');";
     html += "systemData = {";
     html += "status: await statusRes.json(), users: await usersRes.json(), trips: await tripsRes.json(),";
     html += "routes: await routesRes.json(), bus: await busRes.json(), alerts: await alertsRes.json(),";
     html += "stats: await statsRes.json(), health: await healthRes.json()";
     html += "}; ";
     html += "console.log('Data loaded successfully');";
     html += "updateUI();";
     html += "} catch (error) { ";
     html += "console.error('Fetch Error:', error); ";
     html += "document.getElementById('lastUpdate').textContent = 'Error: ' + error.message;";
     html += "document.getElementById('systemHealth').textContent = 'Connection Error';";
     html += "if (error.message.includes('overloaded')) {";
     // Retry after 5 seconds if overloaded
     html += "setTimeout(fetchSystemData, 5000);";
     html += "}";
     html += "}";
     html += "}";
 
     html += "function updateUI() {";
     html += "document.getElementById('passengerCount').textContent = systemData.bus.irPassengerCount || 0;";
     html += "document.getElementById('busPosition').textContent = (systemData.bus.currentLocationKm || 0).toFixed(1) + ' km';";
     html += "document.getElementById('busSpeed').textContent = (systemData.bus.currentSpeedKmph || 0).toFixed(1) + ' km/h';";
     html += "document.getElementById('activeTrips').textContent = systemData.trips.length;";
     html += "document.getElementById('systemHealth').textContent = systemData.health.systemHealth || 'OK';";
     html += "document.getElementById('distance').textContent = (systemData.status.distance || 0).toFixed(0) + ' cm';";
     html += "document.getElementById('totalRevenue').textContent = (systemData.stats.totalRevenue || 0).toFixed(0);";
     html += "document.getElementById('totalUsers').textContent = systemData.stats.totalUsers || 0;";
     html += "document.getElementById('completedTrips').textContent = systemData.stats.completedTrips || 0;";
     html += "document.getElementById('systemUptime').textContent = systemData.stats.uptimeString || '0:00:00';";
     html += "document.getElementById('memoryUsage').textContent = (systemData.stats.freeMemory || 0) + ' bytes';";
     html += "document.getElementById('lastUpdate').textContent = new Date().toLocaleTimeString();";
     html += "document.getElementById('currentSpeed').textContent = (systemData.bus.currentSpeedKmph || 0).toFixed(1) + ' km/h';";
     html += "updateRouteProgress(); updateUsersList(); updateTripsList(); updateAlertsList();";
     html += "}";
 
     html += "function updateRouteProgress() {";
     html += "const currentRoute = Object.values(systemData.routes).find(r => r.id === systemData.bus.routeId);";
     html += "if (currentRoute) {";
     html += "const progress = Math.min((systemData.bus.currentLocationKm / currentRoute.totalDistance) * 100, 100);";
     html += "document.getElementById('routeProgress').textContent = progress.toFixed(0) + '%';";
     html += "document.getElementById('progressFill').style.width = progress + '%';";
     html += "document.getElementById('busSlider').style.left = progress + '%';";
     html += "document.getElementById('routeInfo').textContent = currentRoute.name;";
     html += "document.getElementById('totalDistance').textContent = currentRoute.totalDistance;";
     html += "updateDetailedRouteStops(currentRoute, systemData.bus.currentLocationKm);";
     html += "updateNextStopInfo(currentRoute, systemData.bus.currentLocationKm);";
     html += "}";
     html += "document.getElementById('routeSelect').value = systemData.bus.routeId;";
     html += "}";
 
     html += "function updateDetailedRouteStops(route, currentPosition) {";
     html += "const routeStops = document.getElementById('routeStops');";
     html += "const stopsList = document.getElementById('stopsList');";
     html += "if (!route || !route.stops) return;";
     html += "let stopsHtml = '';";
     html += "let stopsListHtml = '';";
     html += "route.stops.forEach((stop, index) => {";
     html += "const stopProgress = (stop.km / route.totalDistance) * 100;";
     html += "let stopClass = 'route-stop';";
     html += "let stopNameClass = 'stop-name';";
     html += "if (stop.km < currentPosition - 1) { stopClass += ' passed'; }";
     html += "else if (Math.abs(stop.km - currentPosition) <= 1) { stopClass += ' current'; stopNameClass += ' current'; }";
     html += "else { stopClass += ' upcoming'; }";
     html += "if (index === 0) stopNameClass += ' start';";
     html += "if (index === route.stops.length - 1) stopNameClass += ' end';";
     html += "stopsHtml += '<div class=\"' + stopClass + '\" style=\"left: ' + stopProgress + '%;\" title=\"' + stop.stopName + ' (' + stop.km + 'km)\"></div>';";
     html += "stopsListHtml += '<span class=\"' + stopNameClass + '\">' + stop.stopName + '</span>';";
     html += "});";
     html += "routeStops.innerHTML = stopsHtml;";
     html += "stopsList.innerHTML = stopsListHtml;";
     html += "}";
 
     html += "function updateNextStopInfo(route, currentPosition) {";
     html += "const nextStopSpan = document.getElementById('nextStopInfo');";
     html += "if (!route || !route.stops) {";
     html += "nextStopSpan.textContent = 'Next Stop: --';";
     html += "return;";
     html += "}";
     html += "let nextStop = null;";
     html += "for (let stop of route.stops) {";
     html += "if (stop.km > currentPosition) {";
     html += "nextStop = stop;";
     html += "break;";
     html += "}";
     html += "}";
     html += "if (nextStop) {";
     html += "const distance = (nextStop.km - currentPosition).toFixed(1);";
     html += "if (distance <= 2.0) {";
     html += "nextStopSpan.textContent = 'Next: ' + nextStop.stopName + ' (' + distance + 'km)';";
     html += "nextStopSpan.style.color = '#FF5722';";
     html += "nextStopSpan.style.fontWeight = 'bold';";
     html += "} else {";
     html += "nextStopSpan.textContent = 'Next: ' + nextStop.stopName;";
     html += "nextStopSpan.style.color = '#666';";
     html += "nextStopSpan.style.fontWeight = 'normal';";
     html += "}";
     html += "} else {";
     html += "nextStopSpan.textContent = 'Final Stop Reached';";
     html += "nextStopSpan.style.color = '#4CAF50';";
     html += "nextStopSpan.style.fontWeight = 'bold';";
     html += "}";
     html += "}";
 
     html += "function updateUsersList() {";
     html += "const userList = document.getElementById('userList');";
     html += "userList.innerHTML = '';";
     html += "const editSelect = document.getElementById('editUserSelect');";
     html += "const deleteSelect = document.getElementById('deleteUserSelect');";
     html += "editSelect.innerHTML = '<option value=\"\">Select User to Edit</option>';";
     html += "deleteSelect.innerHTML = '<option value=\"\">Select User to Delete</option>';";
     html += "systemData.users.forEach(user => {";
     html += "const li = document.createElement('li');";
     html += "li.className = 'user-item';";
     html += "const statusIcon = user.isBlocked ? '' : '';";
     html += "const addBalanceBtn = `<button class='btn' onclick='addUserBalance(\"${user.uid}\")' style='font-size: 12px; padding: 5px 10px; margin-left: 10px;'>Add Balance</button>`;";
     html += "li.innerHTML = `<strong>${statusIcon} ${user.name}</strong><br>Type: ${user.userType}<br>Balance: ৳${user.balance.toFixed(2)}<br>Trips: ${user.totalTrips || 0} ${addBalanceBtn}`;";
     html += "userList.appendChild(li);";
     html += "editSelect.innerHTML += `<option value='${user.uid}'>${user.name} (${user.uid})</option>`;";
     html += "deleteSelect.innerHTML += `<option value='${user.uid}'>${user.name} (${user.uid})</option>`;";
     html += "});";
     html += "}";
 
     html += "function updateTripsList() {";
     html += "const tripList = document.getElementById('tripList');";
     html += "tripList.innerHTML = '<h4>Active Trips</h4>';";
     html += "if (systemData.trips.length === 0) {";
     html += "tripList.innerHTML += '<p style=\"color: #666;\">No active trips</p>';";
     html += "} else {";
     html += "systemData.trips.forEach(trip => {";
     html += "const div = document.createElement('div');";
     html += "div.className = 'trip-item';";
     html += "const startTime = new Date(trip.startTime).toLocaleTimeString();";
     html += "div.innerHTML = `<strong>${trip.userName}</strong><br>Route: ${trip.routeId}<br>Started: ${startTime}<br>From: ${trip.startLocationKm.toFixed(1)} km`;";
     html += "tripList.appendChild(div);";
     html += "});";
     html += "}";
     html += "}";
 
     html += "function updateAlertsList() {";
     html += "const alertsList = document.getElementById('alertsList');";
     html += "alertsList.innerHTML = '<h4>System Alerts</h4>';";
     html += "if (systemData.alerts.length === 0) {";
     html += "alertsList.innerHTML += '<p style=\"color: #666;\">No active alerts</p>';";
     html += "} else {";
     html += "systemData.alerts.slice(-5).forEach(alert => {";
     html += "const div = document.createElement('div');";
     html += "div.className = `alert alert-${alert.type.toLowerCase()}`;";
     html += "div.innerHTML = `<strong>[${alert.type}]</strong> ${alert.message}<br><small>${alert.timeString}</small>`;";
     html += "alertsList.appendChild(div);";
     html += "});";
     html += "}";
     html += "}";
 
     html += "async function selectRoute() {";
     html += "const routeId = document.getElementById('routeSelect').value;";
     html += "try {";
     html += "await fetch('/api/select_route', {";
     html += "method: 'POST', headers: {'Content-Type': 'application/json'},";
     html += "body: JSON.stringify({route_id: routeId})";
     html += "}); fetchSystemData();";
     html += "} catch (error) { console.error('Error:', error); }";
     html += "}";
 
     html += "async function emergencyStop() {";
     html += "if (confirm('Activate Emergency Stop?')) {";
     html += "try {";
     html += "await fetch('/api/emergency_stop', {method: 'POST'});";
     html += "fetchSystemData();";
     html += "} catch (error) { console.error('Error:', error); }";
     html += "}";
     html += "}";
 
     html += "async function clearAlerts() {";
     html += "try {";
     html += "await fetch('/api/clear_alerts', {method: 'POST'});";
     html += "fetchSystemData();";
     html += "} catch (error) { console.error('Error:', error); }";
     html += "}";
 
     html += "async function departureWarning() {";
     html += "if (confirm('Activate departure warning? This will sound the departure alarm and show warning lights.')) {";
     html += "try {";
     html += "await fetch('/api/departure_warning', {method: 'POST'});";
     html += "alert('Departure warning activated!');";
     html += "} catch (error) { console.error('Error:', error); alert('Error activating departure warning'); }";
     html += "}";
     html += "}";
 
     html += "async function celebrateMilestone() {";
     html += "try {";
     html += "await fetch('/api/celebrate_milestone', {method: 'POST'});";
     html += "alert('Milestone celebration activated!');";
     html += "} catch (error) { console.error('Error:', error); alert('Error activating celebration'); }";
     html += "}";
 
     html += "async function systemReset() {";
     html += "if (confirm('Reset system counters? This will clear active trips and alerts.')) {";
     html += "try {";
     html += "await fetch('/api/system_reset', {method: 'POST'});";
     html += "fetchSystemData();";
     html += "} catch (error) { console.error('Error:', error); }";
     html += "}";
     html += "}";
 
     html += "function showAddUserForm() { document.getElementById('addUserForm').style.display = 'block'; }";
     html += "function hideAddUserForm() { ";
     html += "document.getElementById('addUserForm').style.display = 'none'; ";
     html += "cancelNFCRegistration(); ";
     html += "clearUserForm(); ";
     html += "}";
     html += "function clearUserForm() {";
     html += "document.getElementById('newUserName').value = '';";
     html += "document.getElementById('newUserUID').value = '';";
     html += "document.getElementById('newUserPhone').value = '';";
     html += "document.getElementById('newUserAddress').value = '';";
     html += "document.getElementById('newUserType').value = 'General';";
     html += "document.getElementById('newUserBalance').value = '100';";
     html += "document.getElementById('nfcStatus').style.display = 'none';";
     html += "}";
 
     // Enhanced user management functions
     html += "function showEditUserForm() { document.getElementById('editUserForm').style.display = 'block'; }";
     html += "function hideEditUserForm() { document.getElementById('editUserForm').style.display = 'none'; }";
     html += "function showDeleteUserForm() { document.getElementById('deleteUserForm').style.display = 'block'; }";
     html += "function hideDeleteUserForm() { document.getElementById('deleteUserForm').style.display = 'none'; }";
 
     html += "function loadUserForEdit() {";
     html += "const uid = document.getElementById('editUserSelect').value;";
     html += "if (!uid) return;";
     html += "const user = systemData.users.find(u => u.uid === uid);";
     html += "if (user) {";
     html += "document.getElementById('editUserName').value = user.name;";
     html += "document.getElementById('editUserPhone').value = user.phoneNumber || '';";
     html += "document.getElementById('editUserAddress').value = user.address || '';";
     html += "document.getElementById('editUserType').value = user.userType;";
     html += "document.getElementById('editUserBalance').value = user.balance.toFixed(2);";
     html += "}";
     html += "}";
 
     html += "async function updateUser() {";
     html += "const uid = document.getElementById('editUserSelect').value;";
     html += "const name = document.getElementById('editUserName').value;";
     html += "const phone = document.getElementById('editUserPhone').value;";
     html += "const address = document.getElementById('editUserAddress').value;";
     html += "const userType = document.getElementById('editUserType').value;";
     html += "const balance = parseFloat(document.getElementById('editUserBalance').value) || 0;";
     html += "if (!uid || !name) { alert('Please select a user and fill in the name'); return; }";
     html += "try {";
     html += "await fetch('/api/edit_user', {";
     html += "method: 'POST', headers: {'Content-Type': 'application/json'},";
     html += "body: JSON.stringify({uid, name, phoneNumber: phone, address, userType, balance})";
     html += "}); hideEditUserForm(); fetchSystemData(); alert('User updated successfully!');";
     html += "} catch (error) { console.error('Error:', error); alert('Error updating user'); }";
     html += "}";
 
     html += "async function deleteUser() {";
     html += "const uid = document.getElementById('deleteUserSelect').value;";
     html += "if (!uid) { alert('Please select a user to delete'); return; }";
     html += "const user = systemData.users.find(u => u.uid === uid);";
     html += "if (confirm(`Are you sure you want to delete ${user.name}? This action cannot be undone!`)) {";
     html += "try {";
     html += "await fetch('/api/delete_user', {";
     html += "method: 'POST', headers: {'Content-Type': 'application/json'},";
     html += "body: JSON.stringify({uid})";
     html += "}); hideDeleteUserForm(); fetchSystemData(); alert('User deleted successfully!');";
     html += "} catch (error) { console.error('Error:', error); alert('Error deleting user'); }";
     html += "}";
     html += "}";
 
     html += "async function addUserBalance(uid) {";
     html += "const amount = prompt('Enter amount to add (Tk):');";
     html += "if (amount && !isNaN(amount) && parseFloat(amount) > 0) {";
     html += "try {";
     html += "await fetch('/api/add_balance', {";
     html += "method: 'POST', headers: {'Content-Type': 'application/json'},";
     html += "body: JSON.stringify({uid, amount: parseFloat(amount)})";
     html += "}); fetchSystemData(); alert(`Balance added successfully!`);";
     html += "} catch (error) { console.error('Error:', error); alert('Error adding balance'); }";
     html += "}";
     html += "}";
 
     html += "async function saveAllData() {";
     html += "try {";
     html += "await fetch('/api/save_data', {method: 'POST'});";
     html += "alert('All data saved to persistent storage!');";
     html += "} catch (error) { console.error('Error:', error); alert('Error saving data'); }";
     html += "}";
 
     html += "async function clearAllData() {";
     html += "if (confirm('Are you sure you want to clear ALL persistent data? This will delete all registered users!')) {";
     html += "try {";
     html += "await fetch('/api/clear_data', {method: 'POST'});";
     html += "fetchSystemData(); alert('All persistent data cleared!');";
     html += "} catch (error) { console.error('Error:', error); alert('Error clearing data'); }";
     html += "}";
     html += "}";
 
     // Trip event monitoring for popups
     html += "let lastTripEventTime = 0;";
     html += "setInterval(checkTripEvents, 3000);"; // Check every 3 seconds
     html += "async function checkTripEvents() {";
     html += "try {";
     html += "const response = await fetch('/api/trip_events');";
     html += "const events = await response.json();";
     html += "events.forEach(event => {";
     html += "if (event.timestamp > lastTripEventTime) {";
     html += "showTripPopup(event);";
     html += "lastTripEventTime = event.timestamp;";
     html += "}";
     html += "});";
     html += "} catch (error) { console.error('Trip event check error:', error); }";
     html += "}";
 
     html += "function showTripPopup(event) {";
     html += "const modal = document.getElementById('tripModal');";
     html += "const title = document.getElementById('modalTitle');";
     html += "const content = document.getElementById('modalContent');";
     html += "if (event.type === 'trip_start') {";
     html += "title.textContent = 'Trip Started';";
     html += "title.style.color = '#28a745';";
     html += "content.innerHTML = `";
     html += "<div style='text-align: left;'>";
     html += "<p><strong>Passenger:</strong> ${event.userName}</p>";
     html += "<p><strong>User Type:</strong> ${event.userType}</p>";
     html += "<p><strong>Route:</strong> ${event.routeId}</p>";
     html += "<p><strong>Start Location:</strong> ${event.startLocationKm.toFixed(1)} km</p>";
     html += "<p><strong>Current Balance:</strong> ৳${event.balance.toFixed(2)}</p>";
     html += "<p><strong>Time:</strong> ${new Date(event.timestamp).toLocaleTimeString()}</p>";
     html += "</div>`;";
     html += "} else if (event.type === 'trip_end') {";
     html += "title.textContent = 'Trip Completed';";
     html += "title.style.color = '#007bff';";
     html += "content.innerHTML = `";
     html += "<div style='text-align: left;'>";
     html += "<p><strong>Passenger:</strong> ${event.userName}</p>";
     html += "<p><strong>User Type:</strong> ${event.userType}</p>";
     html += "<p><strong>Distance:</strong> ${event.distance.toFixed(1)} km</p>";
     html += "<p><strong>Fare Charged:</strong> ৳${event.fare.toFixed(2)}</p>";
     html += "<p><strong>Remaining Balance:</strong> ৳${event.balance.toFixed(2)}</p>";
     html += "<p><strong>Duration:</strong> ${event.duration} minutes</p>";
     html += "<p><strong>Completed:</strong> ${new Date(event.timestamp).toLocaleTimeString()}</p>";
     html += "${event.discount > 0 ? `<p style='color: #28a745;'><strong>Discount Applied:</strong> ${event.discount}%</p>` : ''}";
     html += "</div>`;";
     html += "}";
     html += "modal.style.display = 'block';";
     // Auto close after 8 seconds
     html += "setTimeout(() => { if (modal.style.display === 'block') modal.style.display = 'none'; }, 8000);";
     html += "}";
 
     html += "function closeTripModal() { document.getElementById('tripModal').style.display = 'none'; }";
 
     // UID Popup Functions
     html += "let currentUID = '';";
     html += "function showUIDPopup(uid, type) {";
     html += "currentUID = uid;";
     html += "const modal = document.getElementById('uidModal');";
     html += "const title = document.getElementById('uidModalTitle');";
     html += "const content = document.getElementById('uidModalContent');";
     html += "const copyBtn = document.getElementById('copyUIDBtn');";
     html += "if (type === 'success') {";
     html += "title.innerHTML = 'NFC Card UID Read';";
     html += "title.style.color = '#28a745';";
     html += "content.innerHTML = `";
     html += "<div style='background: #d4edda; border: 2px solid #c3e6cb; border-radius: 8px; padding: 15px; margin: 10px 0;'>";
     html += "<p style='margin: 0; font-weight: bold; color: #155724;'>Successfully Read UID:</p>";
     html += "<p style='font-family: monospace; font-size: 20px; font-weight: bold; color: #155724; background: white; padding: 10px; border-radius: 5px; margin: 10px 0; word-break: break-all;'>${uid}</p>";
     html += "<p style='margin: 0; color: #155724;'>This UID has been automatically filled in the registration form. You can also copy it manually if needed.</p>";
     html += "</div>`;";
     html += "copyBtn.style.display = 'inline-block';";
     html += "} else if (type === 'error') {";
     html += "title.innerHTML = 'Registration Error';";
     html += "title.style.color = '#dc3545';";
     html += "content.innerHTML = `";
     html += "<div style='background: #f8d7da; border: 2px solid #f5c6cb; border-radius: 8px; padding: 15px; margin: 10px 0;'>";
     html += "<p style='margin: 0; font-weight: bold; color: #721c24;'>${uid}</p>";
     html += "<p style='margin: 10px 0; color: #721c24;'>Please try with a different NFC card that hasn't been registered yet.</p>";
     html += "</div>`;";
     html += "copyBtn.style.display = 'none';";
     html += "}";
     html += "modal.style.display = 'block';";
     // Auto close after 12 seconds
     html += "setTimeout(() => { if (modal.style.display === 'block') modal.style.display = 'none'; }, 12000);";
     html += "}";
     html += "function closeUIDModal() { document.getElementById('uidModal').style.display = 'none'; }";
     html += "async function copyUIDToClipboard() {";
     html += "try {";
     html += "await navigator.clipboard.writeText(currentUID);";
     html += "const copyBtn = document.getElementById('copyUIDBtn');";
     html += "const originalText = copyBtn.textContent;";
     html += "copyBtn.textContent = 'Copied!';";
     html += "copyBtn.style.background = '#28a745';";
     html += "setTimeout(() => {";
     html += "copyBtn.textContent = originalText;";
     html += "copyBtn.style.background = '#28a745';";
     html += "}, 2000);";
     html += "} catch (error) {";
     html += "console.error('Error copying UID:', error);";
     html += "/* Fallback for older browsers */";
     html += "const textArea = document.createElement('textarea');";
     html += "textArea.value = currentUID;";
     html += "document.body.appendChild(textArea);";
     html += "textArea.select();";
     html += "document.execCommand('copy');";
     html += "document.body.removeChild(textArea);";
     html += "const copyBtn = document.getElementById('copyUIDBtn');";
     html += "copyBtn.textContent = '✅ Copied!';";
     html += "setTimeout(() => { copyBtn.textContent = '📋 Copy UID'; }, 2000);";
     html += "}";
     html += "}";
 
     html += "async function addUser() {";
     html += "const name = document.getElementById('newUserName').value;";
     html += "const uid = document.getElementById('newUserUID').value;";
     html += "const phone = document.getElementById('newUserPhone').value;";
     html += "const address = document.getElementById('newUserAddress').value;";
     html += "const userType = document.getElementById('newUserType').value;";
     html += "const balance = parseFloat(document.getElementById('newUserBalance').value) || 0;";
     html += "if (!name || !uid) { alert('Please fill in name and UID'); return; }";
     html += "try {";
     html += "await fetch('/api/register_user', {";
     html += "method: 'POST', headers: {'Content-Type': 'application/json'},";
     html += "body: JSON.stringify({name, uid, phoneNumber: phone, address, userType, balance})";
     html += "}); hideAddUserForm(); fetchSystemData(); alert('User registered successfully!');";
     html += "} catch (error) { console.error('Error:', error); alert('Error registering user'); }";
     html += "}";
 
     html += "function refreshData() { fetchSystemData(); }";
     html += "function downloadAnalytics() { alert('Analytics download feature coming soon!'); }";
 
     html += "startAutoRefresh();";
     html += "</script></body></html>";
 
     return html;
 }
 
 // API Handlers
 void handleApiStatus()
 {
     server.sendHeader("Access-Control-Allow-Origin", "*");
 
     // Enhanced memory check with early return
     uint32_t freeHeap = ESP.getFreeHeap();
     if (freeHeap < 15000)
     { // Increased threshold
         Serial.println("❌ CRITICAL: Low memory for status API, heap: " + String(freeHeap));
         server.send(503, "application/json", "{\"error\":\"server_overloaded\",\"heap\":" + String(freeHeap) + "}");
         return;
     }
 
     // Use smaller, static JSON response to save memory
     String response = "{";
     response += "\"distance\":" + String(currentDistanceCm) + ",";
     response += "\"passengers\":" + String(passengerCount) + ",";
     response += "\"timestamp\":" + String(millis()) + ",";
     response += "\"nfcRegistrationMode\":" + String(nfcRegistrationMode ? "true" : "false") + ",";
     response += "\"systemInitialized\":" + String(systemInitialized ? "true" : "false") + ",";
     response += "\"freeMemory\":" + String(freeHeap) + ",";
     response += "\"wifiClients\":" + String(WiFi.softAPgetStationNum()) + ",";
     response += "\"uptime\":" + String(millis() - systemStartTime) + ",";
     response += "\"sensorStatus\":\"" + String((currentDistanceCm > 0 && currentDistanceCm < 400) ? "OK" : "ERROR") + "\",";
     response += "\"nfcStatus\":\"" + String((millis() - lastNfcRead < 300000) ? "OK" : "INACTIVE") + "\"";
     response += "}";
 
     server.send(200, "application/json", response);
     stats.webRequests++;
 }
 
 void handleApiUsers()
 {
     server.sendHeader("Access-Control-Allow-Origin", "*");
 
     // Enhanced memory and request throttling
     static unsigned long lastUsersRequest = 0;
     if (millis() - lastUsersRequest < 500)
     { // Min 500ms between requests
         server.send(429, "application/json", "{\"error\":\"rate_limited\"}");
         return;
     }
     lastUsersRequest = millis();
 
     uint32_t freeHeap = ESP.getFreeHeap();
     if (freeHeap < 20000)
     { // Higher threshold for users API
         Serial.println("❌ CRITICAL: Low memory for users API, heap: " + String(freeHeap));
         server.send(503, "application/json", "{\"error\":\"server_overloaded\",\"heap\":" + String(freeHeap) + "}");
         return;
     }
 
     // Use manual JSON construction to save memory
     String response = "[";
     int userCount = 0;
 
     for (const auto &user : users)
     {
         if (userCount >= 15)
             break; // Reduced limit for memory safety
 
         if (userCount > 0)
             response += ",";
         response += "{";
         response += "\"uid\":\"" + user.uid + "\",";
         response += "\"name\":\"" + user.name + "\",";
         response += "\"userType\":\"" + user.userType + "\",";
         response += "\"balance\":" + String(user.balance, 2) + ",";
         response += "\"isRegistered\":" + String(user.isRegistered ? "true" : "false") + ",";
         response += "\"totalTrips\":" + String(user.totalTrips) + ",";
         response += "\"totalSpent\":" + String(user.totalSpent, 2) + ",";
         response += "\"phoneNumber\":\"" + user.phoneNumber + "\",";
         response += "\"address\":\"" + user.address + "\",";
         response += "\"isBlocked\":" + String(user.isBlocked ? "true" : "false") + ",";
         response += "\"registrationDate\":\"" + user.registrationDate + "\",";
         response += "\"lastTapTime\":" + String(user.lastTapTime);
         response += "}";
         userCount++;
 
         // Check memory during construction
         if (ESP.getFreeHeap() < 15000)
         {
             Serial.println("⚠️ Memory running low during users response, stopping at " + String(userCount) + " users");
             break;
         }
     }
 
     response += "]";
 
     server.send(200, "application/json", response);
     stats.webRequests++;
     Serial.println("✅ Users API served: " + String(userCount) + " users, heap: " + String(ESP.getFreeHeap()));
 }
 
 void handleApiTrips()
 {
     server.sendHeader("Access-Control-Allow-Origin", "*");
 
     // Request throttling and memory check
     static unsigned long lastTripsRequest = 0;
     if (millis() - lastTripsRequest < 300)
     { // Min 300ms between requests
         server.send(429, "application/json", "{\"error\":\"rate_limited\"}");
         return;
     }
     lastTripsRequest = millis();
 
     uint32_t freeHeap = ESP.getFreeHeap();
     if (freeHeap < 12000)
     {
         server.send(503, "application/json", "{\"error\":\"server_overloaded\"}");
         return;
     }
 
     // Manual JSON construction for better memory control
     String response = "[";
     int tripCount = 0;
 
     for (const auto &trip : activeTrips)
     {
         if (tripCount >= 8)
             break; // Reduced limit
 
         if (tripCount > 0)
             response += ",";
         response += "{";
         response += "\"uid\":\"" + trip.uid + "\",";
         response += "\"userName\":\"" + trip.userName + "\",";
         response += "\"routeId\":\"" + trip.routeId + "\",";
         response += "\"busId\":\"" + trip.busId + "\",";
         response += "\"startLocationKm\":" + String(trip.startLocationKm, 1) + ",";
         response += "\"startTime\":" + String(trip.startTime) + ",";
         response += "\"isActive\":" + String(trip.isActive ? "true" : "false") + ",";
         response += "\"tripStatus\":\"" + trip.tripStatus + "\",";
         response += "\"fare\":" + String(trip.fare, 2) + ",";
         response += "\"distance\":" + String(trip.distance, 1);
         response += "}";
         tripCount++;
     }
 
     response += "]";
 
     server.send(200, "application/json", response);
     stats.webRequests++;
 }
 
 void handleApiRoutes()
 {
     server.sendHeader("Access-Control-Allow-Origin", "*");
 
     // Memory check and request throttling
     static unsigned long lastRoutesRequest = 0;
     if (millis() - lastRoutesRequest < 1000)
     { // Min 1 second between requests
         server.send(429, "application/json", "{\"error\":\"rate_limited\"}");
         return;
     }
     lastRoutesRequest = millis();
 
     uint32_t freeHeap = ESP.getFreeHeap();
     if (freeHeap < 15000)
     {
         server.send(503, "application/json", "{\"error\":\"server_overloaded\"}");
         return;
     }
 
     // Manual JSON construction for routes with stops
     String response = "{";
     int routeCount = 0;
 
     for (const auto &route : routes)
     {
         if (routeCount > 0)
             response += ",";
         response += "\"" + route.id + "\":{";
         response += "\"id\":\"" + route.id + "\",";
         response += "\"name\":\"" + route.name + "\",";
         response += "\"totalDistance\":" + String(route.totalDistance, 1) + ",";
         response += "\"farePerKm\":" + String(route.farePerKm, 1) + ",";
         response += "\"color\":\"" + route.color + "\",";
         response += "\"maxSpeed\":" + String(route.maxSpeed) + ",";
         response += "\"stopCount\":" + String(route.stopCount) + ",";
         response += "\"description\":\"" + route.description.substring(0, 50) + "\","; // Truncate long descriptions
         response += "\"isActive\":" + String(route.isActive ? "true" : "false") + ",";
         response += "\"totalRevenue\":" + String(route.totalRevenue, 2) + ",";
         response += "\"totalTrips\":" + String(route.totalTrips) + ",";
 
         // Add stops information
         response += "\"stops\":[";
         for (int i = 0; i < route.stops.size(); i++)
         {
             if (i > 0)
                 response += ",";
             response += "{";
             response += "\"lat\":" + String(route.stops[i].lat, 4) + ",";
             response += "\"lng\":" + String(route.stops[i].lng, 4) + ",";
             response += "\"stopName\":\"" + route.stops[i].stopName + "\",";
             response += "\"km\":" + String(route.stops[i].km, 1);
             response += "}";
         }
         response += "]";
 
         response += "}";
         routeCount++;
     }
 
     response += "}";
 
     server.send(200, "application/json", response);
     stats.webRequests++;
 }
 
 void handleApiBus()
 {
     server.sendHeader("Access-Control-Allow-Origin", "*");
 
     // Enhanced request throttling
     static unsigned long lastBusRequest = 0;
     if (millis() - lastBusRequest < 200)
     { // Min 200ms between requests
         server.send(429, "application/json", "{\"error\":\"rate_limited\"}");
         return;
     }
     lastBusRequest = millis();
 
     // Memory check
     uint32_t freeHeap = ESP.getFreeHeap();
     if (freeHeap < 10000)
     {
         server.send(503, "application/json", "{\"error\":\"server_overloaded\"}");
         return;
     }
 
     // Use efficient string construction instead of JSON library
     String response = "{";
     response += "\"routeId\":\"" + currentBus.routeId + "\",";
     response += "\"busId\":\"" + currentBus.busId + "\",";
     response += "\"currentLocationKm\":" + String(currentBus.currentLocationKm, 1) + ",";
     response += "\"currentSpeedKmph\":" + String(currentBus.currentSpeedKmph, 1) + ",";
     response += "\"irPassengerCount\":" + String(currentBus.irPassengerCount) + ",";
     response += "\"tapInCount\":" + String(currentBus.tapInCount) + ",";
     response += "\"status\":\"" + currentBus.status + "\",";
     response += "\"emergencyMode\":" + String(currentBus.emergencyMode ? "true" : "false") + ",";
     response += "\"speedWarning\":" + String(currentBus.speedWarning ? "true" : "false") + ",";
     response += "\"illegalStopWarning\":" + String(currentBus.illegalStopWarning ? "true" : "false") + ",";
     response += "\"dailyRevenue\":" + String(currentBus.dailyRevenue, 2) + ",";
     response += "\"dailyTrips\":" + String(currentBus.dailyTrips) + ",";
     response += "\"driverName\":\"" + currentBus.driverName + "\",";
     response += "\"driverID\":\"" + currentBus.driverID + "\",";
     response += "\"lastUpdate\":" + String(currentBus.lastUpdate) + ",";
     response += "\"nextStopInfo\":\"" + getNextStopName(currentBus.currentLocationKm) + "\",";
     response += "\"currentStopInfo\":\"" + getCurrentStopInfo(currentBus.currentLocationKm) + "\"";
     response += "}";
 
     server.send(200, "application/json", response);
     stats.webRequests++;
 }
 
 void handleApiSelectRoute()
 {
     server.sendHeader("Access-Control-Allow-Origin", "*");
     if (server.hasArg("plain"))
     {
         DynamicJsonDocument doc(512);
         deserializeJson(doc, server.arg("plain"));
         String routeId = doc["route_id"];
 
         // Validate route
         bool validRoute = false;
         for (const auto &route : routes)
         {
             if (route.id == routeId)
             {
                 validRoute = true;
                 break;
             }
         }
 
         if (validRoute)
         {
             currentBus.routeId = routeId;
             currentBus.currentLocationKm = 0; // Reset position
 
             // Enhanced: Visual and audio feedback for route change
             setLcdLights(LIGHT_ALL);
             // playBuzzer(11); // Route change notification
             delay(1000);
             setLcdLights(LIGHT_WHITE);
 
             server.send(200, "application/json", "{\"success\": true}");
             Serial.println("Route changed to: " + routeId);
         }
         else
         {
             server.send(400, "application/json", "{\"error\": \"Invalid route\"}");
         }
     }
     else
     {
         server.send(400, "application/json", "{\"error\": \"No data\"}");
     }
 }
 
 void handleApiAddBalance()
 {
     server.sendHeader("Access-Control-Allow-Origin", "*");
     if (server.hasArg("plain"))
     {
         DynamicJsonDocument doc(512);
         deserializeJson(doc, server.arg("plain"));
         String uid = doc["uid"];
         float amount = doc["amount"];
 
         User *user = findUser(uid);
         if (user && amount > 0)
         {
             user->balance += amount;
             server.send(200, "application/json", "{\"success\": true}");
             Serial.println("Added " + String(amount) + " Tk to " + user->name);
         }
         else
         {
             server.send(400, "application/json", "{\"error\": \"User not found or invalid amount\"}");
         }
     }
     else
     {
         server.send(400, "application/json", "{\"error\": \"No data\"}");
     }
 }
 
 // Enhanced API Handlers
 void handleApiAlerts()
 {
     server.sendHeader("Access-Control-Allow-Origin", "*");
 
     // Request throttling and memory check
     static unsigned long lastAlertsRequest = 0;
     if (millis() - lastAlertsRequest < 1000)
     { // Min 1 second between requests
         server.send(429, "application/json", "{\"error\":\"rate_limited\"}");
         return;
     }
     lastAlertsRequest = millis();
 
     uint32_t freeHeap = ESP.getFreeHeap();
     if (freeHeap < 10000)
     {
         server.send(503, "application/json", "{\"error\":\"server_overloaded\"}");
         return;
     }
 
     // Manual JSON construction for alerts
     String response = "[";
     int alertCount = 0;
 
     for (const auto &alert : systemAlerts)
     {
         if (alertCount >= 10)
             break; // Limit to 10 most recent alerts
 
         if (alertCount > 0)
             response += ",";
         response += "{";
         response += "\"type\":\"" + alert.type + "\",";
         response += "\"message\":\"" + alert.message.substring(0, 100) + "\","; // Truncate long messages
         response += "\"timestamp\":" + String(alert.timestamp) + ",";
         response += "\"isActive\":" + String(alert.isActive ? "true" : "false") + ",";
         response += "\"severity\":\"" + alert.severity + "\",";
         response += "\"timeString\":\"" + formatTime(alert.timestamp) + "\"";
         response += "}";
         alertCount++;
     }
 
     response += "]";
 
     server.send(200, "application/json", response);
 }
 
 void handleApiStats()
 {
     server.sendHeader("Access-Control-Allow-Origin", "*");
 
     // Request throttling and memory check for stats
     static unsigned long lastStatsRequest = 0;
     if (millis() - lastStatsRequest < 2000)
     { // Min 2 seconds between requests
         server.send(429, "application/json", "{\"error\":\"rate_limited\"}");
         return;
     }
     lastStatsRequest = millis();
 
     uint32_t freeHeap = ESP.getFreeHeap();
     if (freeHeap < 12000)
     {
         server.send(503, "application/json", "{\"error\":\"server_overloaded\"}");
         return;
     }
 
     // Manual JSON construction to save memory
     String response = "{";
     response += "\"uptime\":" + String(stats.uptime) + ",";
     response += "\"totalUsers\":" + String(stats.totalUsers) + ",";
     response += "\"activeTrips\":" + String(stats.activeTrips) + ",";
     response += "\"totalRevenue\":" + String(stats.totalRevenue, 2) + ",";
     response += "\"nfcReadings\":" + String(stats.nfcReadings) + ",";
     response += "\"sensorReadings\":" + String(stats.sensorReadings) + ",";
     response += "\"webRequests\":" + String(stats.webRequests) + ",";
     response += "\"averageSpeed\":" + String(stats.averageSpeed, 1) + ",";
     response += "\"violationCount\":" + String(stats.violationCount) + ",";
     response += "\"systemHealth\":\"" + stats.systemHealth + "\",";
     response += "\"freeMemory\":" + String(freeHeap) + ",";
     response += "\"uptimeString\":\"" + formatTime(stats.uptime) + "\",";
     response += "\"completedTrips\":" + String(completedTrips.size()) + ",";
     response += "\"currentDistance\":" + String(currentDistanceCm, 1) + ",";
     response += "\"currentPassengers\":" + String(passengerCount) + ",";
     response += "\"systemStartTime\":" + String(systemStartTime);
     response += "}";
 
     server.send(200, "application/json", response);
     stats.webRequests++;
 }
 
 void handleApiAnalytics()
 {
     server.sendHeader("Access-Control-Allow-Origin", "*");
     DynamicJsonDocument doc(2048);
 
     // Route analytics
     JsonObject routeAnalytics = doc.createNestedObject("routes");
     for (const auto &route : routes)
     {
         JsonObject routeObj = routeAnalytics.createNestedObject(route.id);
         routeObj["name"] = route.name;
         routeObj["totalRevenue"] = route.totalRevenue;
         routeObj["totalTrips"] = route.totalTrips;
         routeObj["isActive"] = route.isActive;
         routeObj["avgRevenuePerTrip"] = route.totalTrips > 0 ? route.totalRevenue / route.totalTrips : 0;
     }
 
     // User type analytics
     JsonObject userAnalytics = doc.createNestedObject("userTypes");
     int studentCount = 0, seniorCount = 0, generalCount = 0, driverCount = 0, policeCount = 0;
     float studentRevenue = 0, seniorRevenue = 0, generalRevenue = 0;
 
     for (const auto &user : users)
     {
         if (user.userType == "Student")
         {
             studentCount++;
             studentRevenue += user.totalSpent;
         }
         else if (user.userType == "Senior Citizen")
         {
             seniorCount++;
             seniorRevenue += user.totalSpent;
         }
         else if (user.userType == "General")
         {
             generalCount++;
             generalRevenue += user.totalSpent;
         }
         else if (user.userType == "Driver")
             driverCount++;
         else if (user.userType == "Police")
             policeCount++;
     }
 
     userAnalytics["student"]["count"] = studentCount;
     userAnalytics["student"]["revenue"] = studentRevenue;
     userAnalytics["senior"]["count"] = seniorCount;
     userAnalytics["senior"]["revenue"] = seniorRevenue;
     userAnalytics["general"]["count"] = generalCount;
     userAnalytics["general"]["revenue"] = generalRevenue;
     userAnalytics["driver"]["count"] = driverCount;
     userAnalytics["police"]["count"] = policeCount;
 
     // Trip analytics
     doc["completedTrips"] = completedTrips.size();
     doc["averageTripDistance"] = 0;
     if (completedTrips.size() > 0)
     {
         float totalDistance = 0;
         for (const auto &trip : completedTrips)
         {
             totalDistance += trip.distance;
         }
         doc["averageTripDistance"] = totalDistance / completedTrips.size();
     }
 
     String response;
     serializeJson(doc, response);
     server.send(200, "application/json", response);
 }
 
 void handleApiHealth()
 {
     server.sendHeader("Access-Control-Allow-Origin", "*");
 
     // Simple memory check for health endpoint
     uint32_t freeHeap = ESP.getFreeHeap();
     if (freeHeap < 8000)
     {
         server.send(503, "application/json", "{\"error\":\"server_overloaded\"}");
         return;
     }
 
     // Lightweight health response using string construction
     String response = "{";
     response += "\"systemHealth\":\"" + stats.systemHealth + "\",";
     response += "\"emergencyMode\":" + String(currentBus.emergencyMode ? "true" : "false") + ",";
     response += "\"speedWarning\":" + String(currentBus.speedWarning ? "true" : "false") + ",";
     response += "\"illegalStopWarning\":" + String(currentBus.illegalStopWarning ? "true" : "false") + ",";
     response += "\"alertCount\":" + String(systemAlerts.size()) + ",";
     response += "\"freeMemory\":" + String(freeHeap) + ",";
     response += "\"uptime\":" + String(stats.uptime) + ",";
     response += "\"lastNfcRead\":" + String(millis() - lastNfcRead) + ",";
     response += "\"sensorStatus\":\"" + String((currentDistanceCm > 0 && currentDistanceCm < 400) ? "OK" : "ERROR") + "\",";
     response += "\"wifiConnected\":" + String(WiFi.softAPgetStationNum());
     response += "}";
 
     server.send(200, "application/json", response);
 }
 
 void handleApiEmergencyStop()
 {
     server.sendHeader("Access-Control-Allow-Origin", "*");
 
     currentBus.emergencyMode = true;
     currentBus.status = "EMERGENCY";
 
     // Emergency visual and audio feedback
     setLcdLights(LIGHT_RED);
     // playBuzzer(5); // Emergency sound pattern
 
     addAlert("EMERGENCY", "Manual emergency stop activated", "CRITICAL");
 
     lcd.clear();
     lcd.setCursor(0, 0);
     lcd.print("R:" + String(ESP.getFreeHeap() / 1024) + "kb");
     lcd.setCursor(13, 0);
     lcd.print("P:" + String(passengerCount) + " T:" + String(currentBus.tapInCount));
     lcd.setCursor(0, 1);
     lcd.print("EMERGENCY STOP");
     lcd.setCursor(0, 2);
     lcd.print("ACTIVATED");
     lcd.setCursor(0, 3);
     lcd.print("Speed: " + String(currentBus.currentSpeedKmph, 1) + " km/h");
 
     server.send(200, "application/json", "{\"status\":\"emergency_activated\"}");
     Serial.println("🚨 EMERGENCY STOP ACTIVATED");
 
     // Keep red lights on during emergency
     delay(1000);
 }
 
 void handleApiClearAlerts()
 {
     server.sendHeader("Access-Control-Allow-Origin", "*");
 
     int clearedCount = systemAlerts.size();
     systemAlerts.clear();
     currentBus.emergencyMode = false;
     currentBus.speedWarning = false;
     currentBus.illegalStopWarning = false;
     currentBus.status = "RUNNING";
 
     // Return to normal lighting
     setLcdLights(LIGHT_WHITE);
     // playBuzzer(1); // Success sound
 
     server.send(200, "application/json", "{\"status\":\"alerts_cleared\",\"count\":" + String(clearedCount) + "}");
     Serial.println("✅ Cleared " + String(clearedCount) + " alerts");
 }
 
 void handleApiBlockUser()
 {
     server.sendHeader("Access-Control-Allow-Origin", "*");
     if (server.hasArg("plain"))
     {
         DynamicJsonDocument doc(512);
         deserializeJson(doc, server.arg("plain"));
         String uid = doc["uid"];
         bool blocked = doc["blocked"];
 
         User *user = findUser(uid);
         if (user)
         {
             user->isBlocked = blocked;
             String action = blocked ? "blocked" : "unblocked";
             addAlert("INFO", "User " + action + ": " + user->name, "MEDIUM");
             server.send(200, "application/json", "{\"status\":\"user_" + action + "\"}");
             Serial.println("👤 User " + action + ": " + user->name);
         }
         else
         {
             server.send(400, "application/json", "{\"error\":\"User not found\"}");
         }
     }
     else
     {
         server.send(400, "application/json", "{\"error\":\"No data\"}");
     }
 }
 
 void handleApiRateTrip()
 {
     server.sendHeader("Access-Control-Allow-Origin", "*");
     if (server.hasArg("plain"))
     {
         DynamicJsonDocument doc(512);
         deserializeJson(doc, server.arg("plain"));
         String uid = doc["uid"];
         int rating = doc["rating"];
         String feedback = doc["feedback"];
 
         // Find most recent completed trip for this user
         for (auto &trip : completedTrips)
         {
             if (trip.uid == uid && trip.rating == 0)
             {
                 trip.rating = rating;
                 trip.feedback = feedback;
                 addAlert("INFO", "Trip rated " + String(rating) + " stars by " + trip.userName, "LOW");
                 server.send(200, "application/json", "{\"status\":\"trip_rated\"}");
                 return;
             }
         }
 
         server.send(400, "application/json", "{\"error\":\"No recent trip found\"}");
     }
     else
     {
         server.send(400, "application/json", "{\"error\":\"No data\"}");
     }
 }
 
 void handleApiRegisterUser()
 {
     server.sendHeader("Access-Control-Allow-Origin", "*");
     if (server.hasArg("plain"))
     {
         DynamicJsonDocument doc(512);
         deserializeJson(doc, server.arg("plain"));
 
         String name = doc["name"];
         String uid = doc["uid"];
         String userType = doc["userType"];
         float balance = doc["balance"];
         String phoneNumber = doc["phoneNumber"] | "";
         String address = doc["address"] | "";
 
         // Check if user already exists
         if (findUser(uid))
         {
             server.send(400, "application/json", "{\"error\": \"User already exists\"}");
             return;
         }
 
         // Add new enhanced user
         User newUser;
         newUser.uid = uid;
         newUser.name = name;
         newUser.userType = userType;
         newUser.balance = balance;
         newUser.isRegistered = true;
         newUser.lastTapTime = 0;
         newUser.totalTrips = 0;
         newUser.totalSpent = 0;
         newUser.phoneNumber = phoneNumber;
         newUser.address = address;
         newUser.isBlocked = false;
         newUser.registrationDate = formatTime(millis());
 
         users.push_back(newUser);
         addAlert("INFO", "New user registered: " + name, "LOW");
 
         // Save user data automatically
         saveUserData();
 
         // Enhanced visual and audio feedback for successful registration
         setLcdLights(LIGHT_ALL);
         // playBuzzer(9); // Welcome sound for new user
         delay(1000);
         setLcdLights(LIGHT_WHITE);
         server.send(200, "application/json", "{\"success\": true}");
         Serial.println("✅ New user registered: " + name + " (" + uid + ")");
     }
     else
     {
         server.send(400, "application/json", "{\"error\": \"No data\"}");
     }
 }
 
 void handleApiSystemReset()
 {
     server.sendHeader("Access-Control-Allow-Origin", "*");
 
     // Reset system counters and clear data
     activeTrips.clear();
     systemAlerts.clear();
 
     // Reset bus status
     currentBus.emergencyMode = false;
     currentBus.speedWarning = false;
     currentBus.illegalStopWarning = false;
     currentBus.status = "IDLE";
     currentBus.currentLocationKm = 0.0;
     currentBus.currentSpeedKmph = 0.0;
     currentBus.dailyRevenue = 0.0;
     currentBus.dailyTrips = 0;
 
     // Reset passenger count
     passengerCount = 0;
     currentBus.irPassengerCount = 0;
     currentBus.tapInCount = 0;
 
     // Reset system stats
     stats.activeTrips = 0;
     stats.nfcReadings = 0;
     stats.sensorReadings = 0;
     stats.webRequests = 0;
     stats.violationCount = 0;
     stats.systemHealth = "EXCELLENT";
 
     addAlert("INFO", "System counters reset", "LOW");
     server.send(200, "application/json", "{\"status\":\"system_reset_complete\"}");
     Serial.println("🔄 System counters and data reset");
 
     // Update LCD to show reset confirmation
     lcd.clear();
     lcd.setCursor(0, 0);
     lcd.print("R:" + String(ESP.getFreeHeap() / 1024) + "kb");
     lcd.setCursor(13, 0);
     lcd.print("P:" + String(passengerCount) + " T:" + String(currentBus.tapInCount));
     lcd.setCursor(0, 1);
     lcd.print("SYSTEM RESET");
     lcd.setCursor(0, 2);
     lcd.print("Counters Cleared");
     lcd.setCursor(0, 3);
     lcd.print("System Ready");
 }
 
 // New API Handlers for Enhanced User Management
 void handleApiDeleteUser()
 {
     server.sendHeader("Access-Control-Allow-Origin", "*");
     if (server.hasArg("plain"))
     {
         DynamicJsonDocument doc(512);
         deserializeJson(doc, server.arg("plain"));
         String uid = doc["uid"];
 
         // Find and remove user
         for (auto it = users.begin(); it != users.end(); ++it)
         {
             if (it->uid == uid)
             {
                 String userName = it->name;
                 users.erase(it);
                 saveUserData(); // Save changes to persistent storage
                 addAlert("INFO", "User deleted: " + userName, "MEDIUM");
                 server.send(200, "application/json", "{\"status\":\"user_deleted\"}");
                 Serial.println("🗑️ User deleted: " + userName + " (" + uid + ")");
                 return;
             }
         }
         server.send(400, "application/json", "{\"error\":\"User not found\"}");
     }
     else
     {
         server.send(400, "application/json", "{\"error\":\"No data\"}");
     }
 }
 
 void handleApiEditUser()
 {
     server.sendHeader("Access-Control-Allow-Origin", "*");
     if (server.hasArg("plain"))
     {
         DynamicJsonDocument doc(512);
         deserializeJson(doc, server.arg("plain"));
         String uid = doc["uid"];
         String name = doc["name"];
         String userType = doc["userType"];
         float balance = doc["balance"];
         String phoneNumber = doc["phoneNumber"] | "";
         String address = doc["address"] | "";
 
         // Find and update user
         User *user = findUser(uid);
         if (user)
         {
             user->name = name;
             user->userType = userType;
             user->balance = balance;
             user->phoneNumber = phoneNumber;
             user->address = address;
 
             saveUserData(); // Save changes to persistent storage
             addAlert("INFO", "User updated: " + name, "LOW");
             server.send(200, "application/json", "{\"status\":\"user_updated\"}");
             Serial.println("✏️ User updated: " + name + " (" + uid + ")");
         }
         else
         {
             server.send(400, "application/json", "{\"error\":\"User not found\"}");
         }
     }
     else
     {
         server.send(400, "application/json", "{\"error\":\"No data\"}");
     }
 }
 
 void handleApiSaveData()
 {
     server.sendHeader("Access-Control-Allow-Origin", "*");
     saveUserData();
     server.send(200, "application/json", "{\"status\":\"data_saved\"}");
     Serial.println("💾 All data manually saved to persistent storage");
 }
 
 void handleApiClearData()
 {
     server.sendHeader("Access-Control-Allow-Origin", "*");
     clearUserData();
     users.clear();
     activeTrips.clear();
     completedTrips.clear();
 
     // Initialize with default data
     initializeSystemData();
     saveUserData();
 
     addAlert("WARNING", "All persistent data cleared and reset to defaults", "HIGH");
     server.send(200, "application/json", "{\"status\":\"data_cleared\"}");
     Serial.println("🗑️ All persistent data cleared and reset");
 }
 
 void handleApiTripEvents()
 {
     server.sendHeader("Access-Control-Allow-Origin", "*");
     DynamicJsonDocument doc(2048);
     JsonArray eventArray = doc.to<JsonArray>();
 
     // Return recent trip events for popup display
     for (const auto &event : tripEvents)
     {
         eventArray.add(event);
     }
 
     String response;
     serializeJson(doc, response);
     server.send(200, "application/json", response);
 
     // Clear events after sending (they've been consumed)
     tripEvents.clear();
 }
 
 // Helper function to add trip events for web popup
 void addTripEvent(String type, User *user, Trip *trip, float fare, float distance, int duration, float discount)
 {
     DynamicJsonDocument event(512);
     event["type"] = type;
     event["timestamp"] = millis();
     event["userName"] = user->name;
     event["userType"] = user->userType;
     event["balance"] = user->balance;
 
     if (trip)
     {
         event["routeId"] = trip->routeId;
         event["startLocationKm"] = trip->startLocationKm;
     }
     else
     {
         event["routeId"] = currentBus.routeId;
         event["startLocationKm"] = currentBus.currentLocationKm;
     }
 
     if (type == "trip_end" && trip)
     {
         event["fare"] = fare;
         event["distance"] = distance;
         event["duration"] = duration;
         event["discount"] = discount;
     }
 
     tripEvents.push_back(event);
 
     // Keep only last 10 events
     if (tripEvents.size() > 10)
     {
         tripEvents.erase(tripEvents.begin());
     }
 }
 
 // NFC Registration API Handlers
 void handleApiStartNFCRegistration()
 {
     server.sendHeader("Access-Control-Allow-Origin", "*");
 
     // Enable NFC registration mode
     nfcRegistrationMode = true;
     nfcRegistrationTimeout = millis() + NFC_REGISTRATION_TIMEOUT;
     lastReadUID = "";
 
     // Visual and audio feedback for NFC registration mode
     setLcdLights(LIGHT_ALL);
     // playBuzzer(1); // Start sound
 
     lcd.clear();
     lcd.setCursor(0, 0);
     lcd.print("R:" + String(ESP.getFreeHeap() / 1024) + "kb");
     lcd.setCursor(13, 0);
     lcd.print("P:" + String(passengerCount) + " T:" + String(currentBus.tapInCount));
     lcd.setCursor(0, 1);
     lcd.print("NFC REGISTRATION");
     lcd.setCursor(0, 2);
     lcd.print("Tap card to read");
     lcd.setCursor(0, 3);
     lcd.print("Timeout: 30 seconds");
 
     delay(1000);
     setLcdLights(LIGHT_WHITE);
 
     server.send(200, "application/json", "{\"status\":\"nfc_registration_started\"}");
     Serial.println("📱 NFC registration mode activated");
     addAlert("INFO", "NFC registration mode activated", "LOW");
 }
 
 void handleApiGetNFCRegistrationStatus()
 {
     server.sendHeader("Access-Control-Allow-Origin", "*");
     DynamicJsonDocument doc(512);
 
     doc["mode"] = nfcRegistrationMode;
     doc["uid"] = lastReadUID;
     doc["timeRemaining"] = nfcRegistrationMode ? max(0L, (long)(nfcRegistrationTimeout - millis())) : 0;
 
     String response;
     serializeJson(doc, response);
     server.send(200, "application/json", response);
 
     // Debug output to help troubleshoot
     Serial.println("📡 NFC Status API Response: mode=" + String(nfcRegistrationMode) + ", uid=" + lastReadUID + ", timeRemaining=" + String(doc["timeRemaining"].as<long>()));
 }
 
 void handleApiCancelNFCRegistration()
 {
     server.sendHeader("Access-Control-Allow-Origin", "*");
 
     // Disable NFC registration mode
     nfcRegistrationMode = false;
     lastReadUID = "";
     nfcRegistrationTimeout = 0;
 
     // Visual feedback
     setLcdLights(LIGHT_WHITE);
 
     server.send(200, "application/json", "{\"status\":\"nfc_registration_cancelled\"}");
     Serial.println("📱 NFC registration mode cancelled");
     addAlert("INFO", "NFC registration mode cancelled", "LOW");
 }
 
 // Enhanced API Handlers for LED and Buzzer Features
 void handleApiDepartureWarning()
 {
     server.sendHeader("Access-Control-Allow-Origin", "*");
 
     playDepartureWarning();
 
     server.send(200, "application/json", "{\"status\":\"departure_warning_activated\"}");
     Serial.println("🚌 Departure warning activated via API");
 }
 
 void handleApiCelebrateMilestone()
 {
     server.sendHeader("Access-Control-Allow-Origin", "*");
 
     celebrateSystemMilestone();
 
     server.send(200, "application/json", "{\"status\":\"milestone_celebration_activated\"}");
     Serial.println("🎉 Milestone celebration activated via API");
 }
 