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
 * Hardware Connections (GPIO numbers same for both boards):
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

// Pin Definitions
#define RC522_SS_PIN 5
#define RC522_RST_PIN 4
#define LCD_SDA 21
#define LCD_SCL 22
#define TRIG_PIN 26
#define ECHO_PIN 27
#define IR_ENTER 32
#define IR_EXIT 33

#define OVER_SPEEDING_LED_PIN 14
#define ENTERED_LED_PIN 12
#define EXIT_LED_PIN 13
#define CORRECT_NFC_PIN 34
#define WRONG_NFC_PIN 35
#define BUZZER_PIN 15
#define TEST 2

// Network Configuration
const char *ssid = "SmartBus_WiFi";
const char *password = "12345678";

// Hardware Objects
MFRC522 rfid(RC522_SS_PIN, RC522_RST_PIN);
LiquidCrystal_I2C lcd(0x27, 20, 4);
WebServer server(80);

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
BusStatus currentBus;
SystemStats stats;

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
const unsigned long LCD_UPDATE_INTERVAL = 3500;       // Faster LCD updates
const unsigned long ANALYTICS_UPDATE_INTERVAL = 7000; // Analytics every 5s
const unsigned long ALERT_CHECK_INTERVAL = 1000;      // Check alerts every 1s
const unsigned long IR_TIMEOUT = 1500;                // Shorter IR timeout
const unsigned long HEALTH_CHECK_INTERVAL = 10000;    // System health every 10s

// Enhanced Fare Constants with Dynamic Pricing
const float BASE_RATE_PER_KM = 5.0;
const float MIN_BALANCE = 15.0;         // Reduced minimum balance
const float PEAK_HOUR_MULTIPLIER = 1.5; // Peak hour pricing
const float STUDENT_DISCOUNT = 0.5;     // 50% discount
const float SENIOR_DISCOUNT = 0.35;     // 35% discount
const float LOYALTY_DISCOUNT = 0.1;     // 10% for frequent users

// Speed and Safety Constants
const float MAX_SPEED_LIMIT = 60.0;         // km/h
const float SPEED_WARNING_THRESHOLD = 50.0; // km/h
const float EMERGENCY_STOP_THRESHOLD = 5.0; // cm for emergency stop

// Enhanced Network Configuration
const int maxConnections = 10; // Support multiple clients

void setup()
{
    pinMode(OVER_SPEEDING_LED_PIN, OUTPUT);
    pinMode(ENTERED_LED_PIN, OUTPUT);
    pinMode(EXIT_LED_PIN, OUTPUT);
    pinMode(CORRECT_NFC_PIN, OUTPUT);
    pinMode(WRONG_NFC_PIN, OUTPUT);
    pinMode(TEST, OUTPUT);
    digitalWrite(TEST, HIGH);
    digitalWrite(WRONG_NFC_PIN, HIGH);

    Serial.begin(115200);

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

    // Initialize default data
    initializeSystemData();

    // Start WiFi Access Point
    WiFi.softAP(ssid, password);
    IPAddress IP = WiFi.softAPIP();

    Serial.println("WiFi AP Started");
    Serial.print("AP IP address: ");
    Serial.println(IP);

    // Setup web server routes
    setupWebServer();
    server.begin();

    // Update LCD
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("WiFi AP: SmartBus");
    lcd.setCursor(0, 1);
    lcd.print("IP: " + IP.toString());
    lcd.setCursor(0, 2);
    lcd.print("Password: 12345678");
    lcd.setCursor(0, 3);
    lcd.print("System Ready!");

    delay(5000);

    Serial.println("=== ESP32 SmartBus System Ready ===");
    Serial.println("NFC + Sensors + Web Server Active");
}

void loop()
{
    server.handleClient();

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
    }

    // Check alerts every second
    if (currentTime - lastAlertCheck >= ALERT_CHECK_INTERVAL)
    {
        cleanupOldAlerts();
        lastAlertCheck = currentTime;
    }

    // Perform comprehensive health check every 10 seconds
    static unsigned long lastHealthCheck = 0;
    if (currentTime - lastHealthCheck >= HEALTH_CHECK_INTERVAL)
    {
        performSystemHealthCheck();
        lastHealthCheck = currentTime;
    }

    // Update statistics counters
    stats.sensorReadings++;

    delay(25); // Reduced delay for better responsiveness
}

void highVoltForNSecond(int pin, int time) {
    digitalWrite(pin, HIGH);
    delay(time);
    digitalWrite(pin, LOW);
}

void highVoltForNSecond_(int pin1, int pin2, int time) {
    digitalWrite(pin1, HIGH);
    digitalWrite(pin2, HIGH);
    delay(time);
    digitalWrite(pin1, LOW);
    digitalWrite(pin2, LOW);
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

    // Initialize enhanced routes with comprehensive data
    routes.push_back({"ROUTE_001", "University - City Center", 17.5, 5.0, "#2196F3", 60, 12, "Main university route connecting campuses to downtown", true, 2340.75, 156});
    routes.push_back({"ROUTE_002", "Airport - Old Town", 17.0, 5.0, "#4CAF50", 70, 8, "Express route from Hazrat Shahjalal Airport to historic Old Dhaka", true, 1890.50, 98});
    routes.push_back({"ROUTE_003", "Dhanmondi - Motijheel", 14.0, 5.0, "#FF9800", 50, 15, "Business district connector through residential areas", true, 3125.25, 203});
    routes.push_back({"ROUTE_004", "Gulshan - Uttara", 22.0, 6.0, "#9C27B0", 80, 10, "Premium route connecting upscale neighborhoods", true, 1560.00, 65});
    routes.push_back({"ROUTE_005", "Mirpur - Farmgate", 12.5, 4.5, "#FF5722", 45, 18, "Local route serving residential and commercial hubs", true, 987.25, 87});

    // Initialize enhanced current bus status
    currentBus = {
        "ROUTE_001", "BUS_001_A", 0.0, 0.0, 0, 0, "IDLE", millis(), false, false, false, 0.0, 0, "Ahmad Hassan", "DRV_001"};

    // Initialize system statistics
    stats = {
        0, 6, 0, 0.0, 0, 0, 0, 0.0, 0, "EXCELLENT"};

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
    // Map distance sensor reading to route position
    // 100cm max distance maps to full route
    float maxDistance = 100.0;
    BusRoute *route = getCurrentRoute();
    if (!route)
        return;

    // Constrain distance reading
    float constrainedDistance = constrain(currentDistanceCm, 2, maxDistance);

    // Map to route position
    float newPosition = (constrainedDistance / maxDistance) * route->totalDistance;

    // Calculate speed
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

        Serial.println("✅ Passenger ENTERED. Count: " + String(passengerCount));
        addAlert("INFO", "Passenger entered bus", "LOW");

        highVoltForNSecond_(ENTERED_LED_PIN, BUZZER_PIN, 500);

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

            Serial.println("✅ Passenger EXITED. Count: " + String(passengerCount));
            addAlert("INFO", "Passenger exited bus", "LOW");

            highVoltForNSecond_(EXIT_LED_PIN, BUZZER_PIN, 500);

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
            // Alert for impossible exit
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

    // Process NFC tap
    processNfcTap(uid);

    // Halt PICC
    rfid.PICC_HaltA();
    rfid.PCD_StopCrypto1();
}

void processNfcTap(String uid)
{
    Serial.println("NFC Card detected: " + uid);

    // Find user
    User *user = findUser(uid);
    if (!user)
    {
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
        Serial.println("Unknown card: " + uid);

        highVoltForNSecond_(WRONG_NFC_PIN, BUZZER_PIN, 1000);

        delay(5000);
        return;
    }

    // Check if user has active trip
    Trip *activeTrip = findActiveTrip(uid);

    highVoltForNSecond_(CORRECT_NFC_PIN, BUZZER_PIN, 500);

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
    if (user->balance < MIN_BALANCE)
    {
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
        lcd.print("Minimum: " + String(MIN_BALANCE, 0) + " Tk");
        Serial.println("❌ Insufficient balance for " + user->name + ": " + String(user->balance));
        addAlert("WARNING", "Trip denied - insufficient balance: " + user->name, "MEDIUM");
        return;
    }

    // Check if user is blocked
    if (user->isBlocked)
    {
        lcd.clear();
        lcd.setCursor(0, 0);
        lcd.print("ACCOUNT BLOCKED");
        lcd.setCursor(0, 1);
        lcd.print("Contact Admin");
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

    delay(2000); // Show trip started message
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

    Serial.println("✅ Trip completed for " + user->name);
    Serial.println("   Distance: " + String(distance, 2) + " km");
    Serial.println("   Duration: " + String(tripDurationMinutes, 1) + " minutes");
    Serial.println("   Fare: " + String(totalFare, 2) + " Tk");
    Serial.println("   User Type: " + user->userType);
    Serial.println("   Remaining balance: " + String(user->balance, 2) + " Tk");

    addAlert("INFO", "Trip completed: " + user->name + " - " + String(totalFare, 2) + " Tk", "LOW");
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
    if (currentBus.currentSpeedKmph > MAX_SPEED_LIMIT)
    {
        if (!currentBus.speedWarning)
        {
            highVoltForNSecond_(OVER_SPEEDING_LED_PIN, BUZZER_PIN, 1000);
            currentBus.speedWarning = true;
            addAlert("WARNING", "Speed limit exceeded: " + String(currentBus.currentSpeedKmph) + " km/h", "HIGH");
        }
    }
    else
    {
        currentBus.speedWarning = false;
    }

    // Check for emergency stop condition
    if (currentDistanceCm <= EMERGENCY_STOP_THRESHOLD && currentBus.currentSpeedKmph > 10)
    {
        if (!currentBus.emergencyMode)
        {
            currentBus.emergencyMode = true;
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
        // Route and position info
        lcd.setCursor(0, 1);
        lcd.print("Route: " + currentBus.routeId.substring(6));
        lcd.setCursor(0, 2);
        lcd.print("Position: " + String(currentBus.currentLocationKm, 1) + " km");
        lcd.setCursor(0, 3);
        lcd.print("Speed: " + String(currentBus.currentSpeedKmph, 1) + " km/h");
        break;

    case 1:
        // Distance sensor and system status
        lcd.setCursor(0, 1);
        lcd.print("Distance: " + String(currentDistanceCm, 0) + " cm");
        lcd.setCursor(0, 2);
        lcd.print("Active Trips: " + String(activeTrips.size()));
        lcd.setCursor(0, 3);
        lcd.print("System: " + stats.systemHealth);
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

void setupWebServer()
{
    // Serve main HTML page
    server.on("/", HTTP_GET, []()
              {
        String html = getMainHtmlPage();
        server.send(200, "text/html", html);
        stats.webRequests++; });

    // Enhanced API endpoints
    server.on("/api/status", HTTP_GET, handleApiStatus);
    server.on("/api/users", HTTP_GET, handleApiUsers);
    server.on("/api/trips", HTTP_GET, handleApiTrips);
    server.on("/api/routes", HTTP_GET, handleApiRoutes);
    server.on("/api/bus", HTTP_GET, handleApiBus);
    server.on("/api/alerts", HTTP_GET, handleApiAlerts);
    server.on("/api/stats", HTTP_GET, handleApiStats);
    server.on("/api/analytics", HTTP_GET, handleApiAnalytics);
    server.on("/api/health", HTTP_GET, handleApiHealth);

    // POST endpoints
    server.on("/api/select_route", HTTP_POST, handleApiSelectRoute);
    server.on("/api/add_balance", HTTP_POST, handleApiAddBalance);
    server.on("/api/register_user", HTTP_POST, handleApiRegisterUser);
    server.on("/api/emergency_stop", HTTP_POST, handleApiEmergencyStop);
    server.on("/api/clear_alerts", HTTP_POST, handleApiClearAlerts);
    server.on("/api/block_user", HTTP_POST, handleApiBlockUser);
    server.on("/api/rate_trip", HTTP_POST, handleApiRateTrip);
    server.on("/api/system_reset", HTTP_POST, handleApiSystemReset);

    // Handle CORS for all requests
    server.onNotFound([]()
                      {
        if (server.method() == HTTP_OPTIONS) {
            server.sendHeader("Access-Control-Allow-Origin", "*");
            server.sendHeader("Access-Control-Allow-Methods", "GET,POST,OPTIONS");
            server.sendHeader("Access-Control-Allow-Headers", "Content-Type");
            server.send(200);
        } else {
            server.send(404, "text/plain", "Endpoint not found");
        } });

    Serial.println("✅ Enhanced web server configured with " + String(16) + " endpoints");
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
    html += "select, input { width: 100%; padding: 8px; margin: 10px 0; border-radius: 4px; border: 1px solid #ddd; }";
    html += ".alert { padding: 10px; margin: 10px 0; border-radius: 5px; }";
    html += ".alert-warning { background: #fff3cd; border: 1px solid #ffeaa7; color: #856404; }";
    html += ".alert-danger { background: #f8d7da; border: 1px solid #f5c6cb; color: #721c24; }";
    html += ".alert-info { background: #d1ecf1; border: 1px solid #bee5eb; color: #0c5460; }";
    html += ".progress-bar { background: #e0e0e0; height: 20px; border-radius: 10px; overflow: hidden; margin: 10px 0; }";
    html += ".progress-fill { background: linear-gradient(90deg, #4CAF50, #2196F3); height: 100%; transition: width 0.5s ease; }";
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

    // Main Dashboard
    html += "<div class='dashboard'>";

    // Bus Control Panel
    html += "<div class='card'>";
    html += "<h3>Bus Control Panel</h3>";
    html += "<label>Current Route:</label>";
    html += "<select id='routeSelect' onchange='selectRoute()'>";
    html += "<option value='ROUTE_001'>University - City Center</option>";
    html += "<option value='ROUTE_002'>Airport - Old Town</option>";
    html += "<option value='ROUTE_003'>Dhanmondi - Motijheel</option>";
    html += "<option value='ROUTE_004'>Gulshan - Uttara</option>";
    html += "<option value='ROUTE_005'>Mirpur - Farmgate</option>";
    html += "</select>";

    html += "<div class='sensor-grid'>";
    html += "<div class='sensor-item'><div class='sensor-value' id='distance'>0 cm</div><div>Distance</div></div>";
    html += "<div class='sensor-item'><div class='sensor-value' id='routeProgress'>0%</div><div>Progress</div></div>";
    html += "</div>";

    html += "<div class='progress-bar'><div class='progress-fill' id='progressBar' style='width: 0%'></div></div>";
    html += "<div id='busAlerts'></div>";

    html += "<button class='btn btn-danger' onclick='emergencyStop()'>Emergency Stop</button>";
    html += "<button class='btn btn-warning' onclick='clearAlerts()'>Clear Alerts</button>";
    html += "</div>";

    // Real-time Analytics
    html += "<div class='card'>";
    html += "<h3>Real-time Analytics</h3>";
    html += "<div class='analytics-grid'>";
    html += "<div class='metric'><div class='metric-value' id='totalRevenue'>0</div><div class='metric-label'>Total Revenue (Tk)</div></div>";
    html += "<div class='metric'><div class='metric-value' id='totalUsers'>0</div><div class='metric-label'>Registered Users</div></div>";
    html += "<div class='metric'><div class='metric-value' id='completedTrips'>0</div><div class='metric-label'>Completed Trips</div></div>";
    html += "<div class='metric'><div class='metric-value' id='systemUptime'>0</div><div class='metric-label'>System Uptime</div></div>";
    html += "</div></div>";

    // Enhanced Users Management
    html += "<div class='card'>";
    html += "<h3>Users Management</h3>";
    html += "<ul class='user-list' id='userList'></ul>";
    html += "<button class='btn btn-success' onclick='showAddUserForm()'>Add New User</button>";
    html += "<div id='addUserForm' style='display: none; margin-top: 15px; padding: 15px; background: #f8f9fa; border-radius: 8px;'>";
    html += "<h4>Add New User</h4>";
    html += "<input type='text' id='newUserName' placeholder='Full Name'>";
    html += "<input type='text' id='newUserUID' placeholder='NFC Card UID (8-32 digits)'>";
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
    html += "</div></div>";

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

    // Enhanced JavaScript
    html += "<script>";
    html += "let systemData = {}; let refreshInterval;";
    html += "function startAutoRefresh() { refreshInterval = setInterval(fetchSystemData, 2000); fetchSystemData(); }";
    html += "async function fetchSystemData() {";
    html += "try {";
    html += "const [statusRes, usersRes, tripsRes, routesRes, busRes, alertsRes, statsRes, healthRes] = await Promise.all([";
    html += "fetch('/api/status'), fetch('/api/users'), fetch('/api/trips'), fetch('/api/routes'),";
    html += "fetch('/api/bus'), fetch('/api/alerts'), fetch('/api/stats'), fetch('/api/health')";
    html += "]);";
    html += "systemData = {";
    html += "status: await statusRes.json(), users: await usersRes.json(), trips: await tripsRes.json(),";
    html += "routes: await routesRes.json(), bus: await busRes.json(), alerts: await alertsRes.json(),";
    html += "stats: await statsRes.json(), health: await healthRes.json()";
    html += "}; updateUI();";
    html += "} catch (error) { console.error('Error:', error); }";
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
    html += "updateRouteProgress(); updateUsersList(); updateTripsList(); updateAlertsList();";
    html += "}";

    html += "function updateRouteProgress() {";
    html += "const currentRoute = Object.values(systemData.routes).find(r => r.id === systemData.bus.routeId);";
    html += "if (currentRoute) {";
    html += "const progress = (systemData.bus.currentLocationKm / currentRoute.totalDistance) * 100;";
    html += "document.getElementById('routeProgress').textContent = progress.toFixed(0) + '%';";
    html += "document.getElementById('progressBar').style.width = Math.min(progress, 100) + '%';";
    html += "}";
    html += "document.getElementById('routeSelect').value = systemData.bus.routeId;";
    html += "}";

    html += "function updateUsersList() {";
    html += "const userList = document.getElementById('userList');";
    html += "userList.innerHTML = '';";
    html += "systemData.users.forEach(user => {";
    html += "const li = document.createElement('li');";
    html += "li.className = 'user-item';";
    html += "const statusIcon = user.isBlocked ? 'NO' : 'YES';";
    html += "li.innerHTML = `<strong>${statusIcon} ${user.name}</strong><br>Type: ${user.userType}<br>Balance: ৳${user.balance.toFixed(2)}<br>Trips: ${user.totalTrips || 0}`;";
    html += "userList.appendChild(li);";
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

    html += "async function systemReset() {";
    html += "if (confirm('Reset system counters? This will clear active trips and alerts.')) {";
    html += "try {";
    html += "await fetch('/api/system_reset', {method: 'POST'});";
    html += "fetchSystemData();";
    html += "} catch (error) { console.error('Error:', error); }";
    html += "}";
    html += "}";

    html += "function showAddUserForm() { document.getElementById('addUserForm').style.display = 'block'; }";
    html += "function hideAddUserForm() { document.getElementById('addUserForm').style.display = 'none'; clearUserForm(); }";
    html += "function clearUserForm() {";
    html += "document.getElementById('newUserName').value = '';";
    html += "document.getElementById('newUserUID').value = '';";
    html += "document.getElementById('newUserPhone').value = '';";
    html += "document.getElementById('newUserAddress').value = '';";
    html += "document.getElementById('newUserType').value = 'General';";
    html += "document.getElementById('newUserBalance').value = '100';";
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
    DynamicJsonDocument doc(1024);
    doc["distance"] = currentDistanceCm;
    doc["passengers"] = passengerCount;
    doc["timestamp"] = millis();

    String response;
    serializeJson(doc, response);
    server.send(200, "application/json", response);
}

void handleApiUsers()
{
    server.sendHeader("Access-Control-Allow-Origin", "*");
    DynamicJsonDocument doc(2048);
    JsonArray userArray = doc.to<JsonArray>();

    for (const auto &user : users)
    {
        JsonObject userObj = userArray.createNestedObject();
        userObj["uid"] = user.uid;
        userObj["name"] = user.name;
        userObj["userType"] = user.userType;
        userObj["balance"] = user.balance;
    }

    String response;
    serializeJson(doc, response);
    server.send(200, "application/json", response);
}

void handleApiTrips()
{
    server.sendHeader("Access-Control-Allow-Origin", "*");
    DynamicJsonDocument doc(2048);
    JsonArray tripArray = doc.to<JsonArray>();

    for (const auto &trip : activeTrips)
    {
        JsonObject tripObj = tripArray.createNestedObject();
        tripObj["uid"] = trip.uid;
        tripObj["userName"] = trip.userName;
        tripObj["routeId"] = trip.routeId;
        tripObj["busId"] = trip.busId;
        tripObj["startLocationKm"] = trip.startLocationKm;
        tripObj["startTime"] = trip.startTime;
    }

    String response;
    serializeJson(doc, response);
    server.send(200, "application/json", response);
}

void handleApiRoutes()
{
    server.sendHeader("Access-Control-Allow-Origin", "*");
    DynamicJsonDocument doc(2048);
    JsonObject routeObj = doc.to<JsonObject>();

    for (const auto &route : routes)
    {
        JsonObject r = routeObj.createNestedObject(route.id);
        r["name"] = route.name;
        r["totalDistance"] = route.totalDistance;
        r["farePerKm"] = route.farePerKm;
        r["color"] = route.color;
    }

    String response;
    serializeJson(doc, response);
    server.send(200, "application/json", response);
}

void handleApiBus()
{
    server.sendHeader("Access-Control-Allow-Origin", "*");
    DynamicJsonDocument doc(1024);
    doc["routeId"] = currentBus.routeId;
    doc["currentLocationKm"] = currentBus.currentLocationKm;
    doc["currentSpeedKmph"] = currentBus.currentSpeedKmph;
    doc["irPassengerCount"] = currentBus.irPassengerCount;
    doc["tapInCount"] = currentBus.tapInCount;
    doc["status"] = currentBus.status;

    String response;
    serializeJson(doc, response);
    server.send(200, "application/json", response);
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
    DynamicJsonDocument doc(2048);
    JsonArray alertArray = doc.to<JsonArray>();

    for (const auto &alert : systemAlerts)
    {
        JsonObject alertObj = alertArray.createNestedObject();
        alertObj["type"] = alert.type;
        alertObj["message"] = alert.message;
        alertObj["timestamp"] = alert.timestamp;
        alertObj["isActive"] = alert.isActive;
        alertObj["severity"] = alert.severity;
        alertObj["timeString"] = formatTime(alert.timestamp);
    }

    String response;
    serializeJson(doc, response);
    server.send(200, "application/json", response);
}

void handleApiStats()
{
    server.sendHeader("Access-Control-Allow-Origin", "*");
    DynamicJsonDocument doc(1024);

    doc["uptime"] = stats.uptime;
    doc["totalUsers"] = stats.totalUsers;
    doc["activeTrips"] = stats.activeTrips;
    doc["totalRevenue"] = stats.totalRevenue;
    doc["nfcReadings"] = stats.nfcReadings;
    doc["sensorReadings"] = stats.sensorReadings;
    doc["webRequests"] = stats.webRequests;
    doc["averageSpeed"] = stats.averageSpeed;
    doc["violationCount"] = stats.violationCount;
    doc["systemHealth"] = stats.systemHealth;
    doc["freeMemory"] = ESP.getFreeHeap();
    doc["uptimeString"] = formatTime(stats.uptime);

    String response;
    serializeJson(doc, response);
    server.send(200, "application/json", response);
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
    DynamicJsonDocument doc(1024);

    doc["systemHealth"] = stats.systemHealth;
    doc["emergencyMode"] = currentBus.emergencyMode;
    doc["speedWarning"] = currentBus.speedWarning;
    doc["illegalStopWarning"] = currentBus.illegalStopWarning;
    doc["alertCount"] = systemAlerts.size();
    doc["freeMemory"] = ESP.getFreeHeap();
    doc["uptime"] = stats.uptime;
    doc["lastNfcRead"] = millis() - lastNfcRead;
    doc["sensorStatus"] = (currentDistanceCm > 0 && currentDistanceCm < 400) ? "OK" : "ERROR";
    doc["wifiConnected"] = WiFi.softAPgetStationNum();

    String response;
    serializeJson(doc, response);
    server.send(200, "application/json", response);
}

void handleApiEmergencyStop()
{
    server.sendHeader("Access-Control-Allow-Origin", "*");

    currentBus.emergencyMode = true;
    currentBus.status = "EMERGENCY";
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
