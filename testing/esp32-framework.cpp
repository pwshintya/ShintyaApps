/*
 * ESP32 Package Management System Framework
 * Integrates with React Native + Firebase package tracking application
 * 
 * Features:
 * - RFID card pairing for packages
 * - Real-time capacity monitoring
 * - Package access control
 * - Firebase Realtime Database and Firestore integration
 */

#include <WiFi.h>
#include <FirebaseESP32.h>
#include <ArduinoJson.h>
#include <SPI.h>
#include <MFRC522.h>
#include <LiquidCrystal_I2C.h>
#include <NewPing.h>

// ======================== HARDWARE CONFIGURATION ========================
#define SS_PIN    21
#define RST_PIN   22
#define TRIGGER_PIN  12
#define ECHO_PIN     14
#define MAX_DISTANCE 200
#define BUZZER_PIN   4
#define LED_GREEN    2
#define LED_RED      5

// ======================== WIFI CONFIGURATION ========================
const char* WIFI_SSID = "YOUR_WIFI_SSID";
const char* WIFI_PASSWORD = "YOUR_WIFI_PASSWORD";

// ======================== FIREBASE CONFIGURATION ========================
// Match the configuration from services/firebase.js
#define FIREBASE_HOST "alien-outrider-453003-g8-default-rtdb.firebaseio.com"
#define FIREBASE_AUTH "AIzaSyA5Lsxqplxa4eQ9H8Zap3e95R_-SFGe2yU"
#define PROJECT_ID "alien-outrider-453003-g8"

// ======================== HARDWARE INSTANCES ========================
MFRC522 rfid(SS_PIN, RST_PIN);
LiquidCrystal_I2C lcd(0x27, 16, 2);
NewPing sonar(TRIGGER_PIN, ECHO_PIN, MAX_DISTANCE);
FirebaseData fbdo;
FirebaseAuth auth;
FirebaseConfig config;

// ======================== SYSTEM STATE VARIABLES ========================
struct SystemState {
  bool wifiConnected = false;
  bool firebaseConnected = false;
  bool isProcessing = false;
  String currentSession = "";
  String deviceId = "ESP32_001";
  unsigned long lastHeartbeat = 0;
  unsigned long lastCapacityCheck = 0;
  unsigned long lastStatusCheck = 0;
};

struct PairingSession {
  bool isActive = false;
  String userId = "";
  String generatedRfid = "";
  unsigned long startTime = 0;
  unsigned long timeout = 30000; // 30 seconds timeout
};

struct CapacityData {
  float currentHeight = 0;
  float maxHeight = 30.0; // 30cm maximum capacity
  float percentage = 0;
  String status = "Kosong";
  unsigned long lastUpdate = 0;
};

struct PackageAccess {
  bool isProcessing = false;
  String scannedRfid = "";
  String userId = "";
  unsigned long accessTime = 0;
};

// Global instances
SystemState systemState;
PairingSession pairingSession;
CapacityData capacityData;
PackageAccess packageAccess;

// ======================== TIMING CONSTANTS ========================
const unsigned long HEARTBEAT_INTERVAL = 10000;     // 10 seconds
const unsigned long CAPACITY_CHECK_INTERVAL = 5000;  // 5 seconds
const unsigned long STATUS_CHECK_INTERVAL = 2000;    // 2 seconds
const unsigned long DISPLAY_UPDATE_INTERVAL = 1000;  // 1 second

// ======================== SETUP FUNCTION ========================
void setup() {
  Serial.begin(115200);
  Serial.println("=== ESP32 Package Management System ===");
  
  // Initialize hardware
  initializeHardware();
  
  // Connect to WiFi
  connectToWiFi();
  
  // Initialize Firebase
  initializeFirebase();
  
  // Initialize system state
  initializeSystemState();
  
  Serial.println("System initialization complete!");
  updateDisplay("System Ready", "Waiting...");
}

// ======================== MAIN LOOP ========================
void loop() {
  unsigned long currentTime = millis();
  
  // Check WiFi connection
  if (WiFi.status() != WL_CONNECTED) {
    systemState.wifiConnected = false;
    reconnectWiFi();
    return;
  }
  
  // Heartbeat to Firebase
  if (currentTime - systemState.lastHeartbeat > HEARTBEAT_INTERVAL) {
    sendHeartbeat();
    systemState.lastHeartbeat = currentTime;
  }
  
  // Check capacity sensor
  if (currentTime - systemState.lastCapacityCheck > CAPACITY_CHECK_INTERVAL) {
    checkCapacity();
    systemState.lastCapacityCheck = currentTime;
  }
  
  // Check system status from Firebase
  if (currentTime - systemState.lastStatusCheck > STATUS_CHECK_INTERVAL) {
    checkSystemStatus();
    systemState.lastStatusCheck = currentTime;
  }
  
  // Handle RFID scanning
  handleRFIDScanning();
  
  // Handle pairing session timeout
  handlePairingTimeout();
  
  // Update display
  updateSystemDisplay();
  
  delay(100);
}

// ======================== HARDWARE INITIALIZATION ========================
void initializeHardware() {
  // Initialize SPI for RFID
  SPI.begin();
  rfid.PCD_Init();
  
  // Initialize LCD
  lcd.init();
  lcd.backlight();
  lcd.clear();
  
  // Initialize GPIO pins
  pinMode(BUZZER_PIN, OUTPUT);
  pinMode(LED_GREEN, OUTPUT);
  pinMode(LED_RED, OUTPUT);
  
  // Test hardware
  digitalWrite(LED_GREEN, HIGH);
  delay(500);
  digitalWrite(LED_GREEN, LOW);
  digitalWrite(LED_RED, HIGH);
  delay(500);
  digitalWrite(LED_RED, LOW);
  
  tone(BUZZER_PIN, 1000, 200);
  
  Serial.println("Hardware initialized successfully");
}

// ======================== WIFI CONNECTION ========================
void connectToWiFi() {
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Connecting WiFi");
  
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  
  int attempts = 0;
  while (WiFi.status() != WL_CONNECTED && attempts < 20) {
    delay(1000);
    Serial.print(".");
    lcd.setCursor(attempts % 16, 1);
    lcd.print(".");
    attempts++;
  }
  
  if (WiFi.status() == WL_CONNECTED) {
    systemState.wifiConnected = true;
    Serial.println("\nWiFi Connected!");
    Serial.print("IP Address: ");
    Serial.println(WiFi.localIP());
    
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("WiFi Connected");
    lcd.setCursor(0, 1);
    lcd.print(WiFi.localIP());
    delay(2000);
  } else {
    Serial.println("WiFi connection failed!");
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("WiFi Failed");
    delay(2000);
  }
}

void reconnectWiFi() {
  Serial.println("Reconnecting to WiFi...");
  WiFi.disconnect();
  WiFi.reconnect();
  delay(5000);
}

// ======================== FIREBASE INITIALIZATION ========================
void initializeFirebase() {
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Init Firebase");
  
  config.host = FIREBASE_HOST;
  config.api_key = FIREBASE_AUTH;
  config.database_url = "https://" + String(FIREBASE_HOST);
  
  // Initialize Firebase
  Firebase.begin(&config, &auth);
  Firebase.reconnectWiFi(true);
  
  // Test connection
  if (Firebase.ready()) {
    systemState.firebaseConnected = true;
    Serial.println("Firebase connected successfully!");
    
    lcd.setCursor(0, 1);
    lcd.print("Firebase OK");
    delay(2000);
  } else {
    Serial.println("Firebase connection failed!");
    lcd.setCursor(0, 1);
    lcd.print("Firebase Failed");
    delay(2000);
  }
}

// ======================== SYSTEM STATE INITIALIZATION ========================
void initializeSystemState() {
  // Reset hardware status in Firebase RTDB
  FirebaseJson json;
  json.set("isInUse", false);
  json.set("sessionType", "");
  json.set("deviceId", systemState.deviceId);
  json.set("lastActivity", getCurrentTimestamp());
  json.set("rfid", "");
  json.set("userRfid", "");
  json.set("weight", 0);
  json.set("height", 0);
  json.set("measurementComplete", false);
  
  Firebase.RTDB.setJSON(&fbdo, "/systemStatus/hardware", &json);
  
  // Initialize capacity data in Firestore
  updateCapacityInFirestore(0, 30);
  
  Serial.println("System state initialized");
}

// ======================== SYSTEM STATUS MONITORING ========================
void checkSystemStatus() {
  if (!systemState.firebaseConnected) return;
  
  // Check for pairing sessions
  if (Firebase.RTDB.getJSON(&fbdo, "/rfid_pairing/current_session")) {
    FirebaseJson json = fbdo.jsonObject();
    
    bool isActive = false;
    json.get("isActive", isActive);
    
    if (isActive && !pairingSession.isActive) {
      String userId = "";
      json.get("userId", userId);
      
      if (userId != "") {
        startPairingSession(userId);
      }
    }
  }
  
  // Check for package access requests via RTDB
  if (Firebase.RTDB.getJSON(&fbdo, "/systemStatus/hardware")) {
    FirebaseJson json = fbdo.jsonObject();
    
    bool isInUse = false;
    json.get("isInUse", isInUse);
    
    if (isInUse) {
      String sessionType = "";
      json.get("sessionType", sessionType);
      
      if (sessionType == "package_access") {
        String userRfid = "";
        json.get("userRfid", userRfid);
        
        if (userRfid != "" && !packageAccess.isProcessing) {
          packageAccess.isProcessing = true;
          systemState.currentSession = "package_access";
          updateDisplay("Package Access", "Scan RFID Card");
        }
      }
    }
  }
}

// ======================== RFID PAIRING SESSION ========================
void startPairingSession(String userId) {
  pairingSession.isActive = true;
  pairingSession.userId = userId;
  pairingSession.startTime = millis();
  
  // Generate random RFID code
  pairingSession.generatedRfid = generateRandomRFID();
  
  Serial.println("=== RFID Pairing Session Started ===");
  Serial.println("User ID: " + userId);
  Serial.println("Generated RFID: " + pairingSession.generatedRfid);
  
  // Update Firebase RTDB with generated RFID
  Firebase.RTDB.setString(&fbdo, "/systemStatus/hardware/rfid", pairingSession.generatedRfid);
  Firebase.RTDB.setString(&fbdo, "/systemStatus/hardware/sessionType", "rfid");
  Firebase.RTDB.setBool(&fbdo, "/systemStatus/hardware/isInUse", true);
  Firebase.RTDB.setString(&fbdo, "/systemStatus/hardware/lastActivity", getCurrentTimestamp());
  
  // Update pairing session in Firestore
  updatePairingSession();
  
  // Update display and indicators
  updateDisplay("RFID Pairing", pairingSession.generatedRfid);
  digitalWrite(LED_GREEN, HIGH);
  tone(BUZZER_PIN, 1500, 500);
  
  systemState.currentSession = "pairing";
}

void updatePairingSession() {
  // Update the pairing session document in Firestore
  FirebaseJson json;
  json.set("rfidCode", pairingSession.generatedRfid);
  json.set("status", "generated");
  json.set("receivedTime", getCurrentTimestamp());
  
  Firebase.Firestore.patchDocument(&fbdo, PROJECT_ID, "", "rfid_pairing/current_session", json.raw());
}

void handlePairingTimeout() {
  if (pairingSession.isActive) {
    unsigned long elapsed = millis() - pairingSession.startTime;
    
    if (elapsed > pairingSession.timeout) {
      // Timeout reached
      Serial.println("Pairing session timeout!");
      
      // Cancel pairing session
      cancelPairingSession();
    }
  }
}

void cancelPairingSession() {
  pairingSession.isActive = false;
  pairingSession.userId = "";
  pairingSession.generatedRfid = "";
  pairingSession.startTime = 0;
  
  // Reset Firebase RTDB
  Firebase.RTDB.setBool(&fbdo, "/systemStatus/hardware/isInUse", false);
  Firebase.RTDB.setString(&fbdo, "/systemStatus/hardware/sessionType", "");
  Firebase.RTDB.setString(&fbdo, "/systemStatus/hardware/rfid", "");
  
  // Update pairing session in Firestore
  FirebaseJson json;
  json.set("isActive", false);
  json.set("status", "cancelled");
  json.set("cancelledTime", getCurrentTimestamp());
  
  Firebase.Firestore.patchDocument(&fbdo, PROJECT_ID, "", "rfid_pairing/current_session", json.raw());
  
  digitalWrite(LED_GREEN, LOW);
  systemState.currentSession = "";
  
  Serial.println("Pairing session cancelled");
}

// ======================== RFID SCANNING ========================
void handleRFIDScanning() {
  if (!rfid.PICC_IsNewCardPresent() || !rfid.PICC_ReadCardSerial()) {
    return;
  }
  
  // Read RFID card
  String scannedRfid = "";
  for (byte i = 0; i < rfid.uid.size; i++) {
    scannedRfid += String(rfid.uid.uidByte[i] < 0x10 ? "0" : "");
    scannedRfid += String(rfid.uid.uidByte[i], HEX);
  }
  scannedRfid.toUpperCase();
  
  Serial.println("RFID Scanned: " + scannedRfid);
  
  // Handle based on current session
  if (systemState.currentSession == "package_access") {
    handlePackageAccess(scannedRfid);
  } else {
    // Check if this RFID has access to any packages
    checkPackageAccess(scannedRfid);
  }
  
  rfid.PICC_HaltA();
  rfid.PCD_StopCrypto1();
}

void handlePackageAccess(String scannedRfid) {
  updateDisplay("Processing...", "Please wait");
  
  // Simulate package access validation
  delay(2000);
  
  // For simulation, allow access if RFID matches expected
  bool accessGranted = true; // In real implementation, check against Firestore
  
  if (accessGranted) {
    Serial.println("Package access granted!");
    
    // Log successful access
    logPackageAccess(scannedRfid, "granted");
    
    // Grant access
    digitalWrite(LED_GREEN, HIGH);
    tone(BUZZER_PIN, 2000, 1000);
    updateDisplay("Access Granted", "Take your package");
    
    delay(5000);
    digitalWrite(LED_GREEN, LOW);
    
  } else {
    Serial.println("Package access denied!");
    
    // Log failed access
    logPackageAccess(scannedRfid, "denied");
    
    // Deny access
    digitalWrite(LED_RED, HIGH);
    tone(BUZZER_PIN, 500, 1000);
    updateDisplay("Access Denied", "Invalid RFID");
    
    delay(3000);
    digitalWrite(LED_RED, LOW);
  }
  
  // Reset session
  packageAccess.isProcessing = false;
  systemState.currentSession = "";
  
  // Reset Firebase RTDB
  Firebase.RTDB.setBool(&fbdo, "/systemStatus/hardware/isInUse", false);
  Firebase.RTDB.setString(&fbdo, "/systemStatus/hardware/sessionType", "");
  Firebase.RTDB.setString(&fbdo, "/systemStatus/hardware/userRfid", "");
}

void checkPackageAccess(String scannedRfid) {
  // Check if this RFID has any packages to access
  // This would query Firestore for user packages
  Serial.println("Checking package access for RFID: " + scannedRfid);
  
  // For simulation, allow access for any valid RFID format
  if (scannedRfid.length() == 8) {
    updateDisplay("Welcome!", "Checking packages");
    
    delay(2000);
    
    // Simulate package found
    updateDisplay("Package Found", "Opening box...");
    
    digitalWrite(LED_GREEN, HIGH);
    tone(BUZZER_PIN, 1800, 500);
    
    delay(3000);
    digitalWrite(LED_GREEN, LOW);
    
    logPackageAccess(scannedRfid, "self_access");
    
  } else {
    updateDisplay("Invalid RFID", "Try again");
    digitalWrite(LED_RED, HIGH);
    tone(BUZZER_PIN, 400, 500);
    
    delay(2000);
    digitalWrite(LED_RED, LOW);
  }
}

// ======================== CAPACITY MONITORING ========================
void checkCapacity() {
  float distance = sonar.ping_cm();
  
  if (distance > 0 && distance <= capacityData.maxHeight) {
    capacityData.currentHeight = capacityData.maxHeight - distance;
    capacityData.percentage = (capacityData.currentHeight / capacityData.maxHeight) * 100;
    
    // Determine status
    if (capacityData.percentage < 10) {
      capacityData.status = "Kosong";
    } else if (capacityData.percentage < 50) {
      capacityData.status = "Tersedia";
    } else if (capacityData.percentage < 80) {
      capacityData.status = "Cukup Penuh";
    } else {
      capacityData.status = "Hampir Penuh";
    }
    
    capacityData.lastUpdate = millis();
    
    // Update Firestore every 10 seconds
    static unsigned long lastFirestoreUpdate = 0;
    if (millis() - lastFirestoreUpdate > 10000) {
      updateCapacityInFirestore(capacityData.currentHeight, capacityData.maxHeight);
      lastFirestoreUpdate = millis();
    }
    
    Serial.printf("Capacity: %.1fcm (%.1f%%) - %s\n", 
                  capacityData.currentHeight, 
                  capacityData.percentage, 
                  capacityData.status.c_str());
  }
}

void updateCapacityInFirestore(float height, float maxHeight) {
  FirebaseJson json;
  json.set("height", height);
  json.set("maxHeight", maxHeight);
  json.set("lastUpdated", getCurrentTimestamp());
  json.set("deviceId", systemState.deviceId);
  
  Firebase.Firestore.patchDocument(&fbdo, PROJECT_ID, "", "capacity/box_sensor", json.raw());
}

// ======================== ACTIVITY LOGGING ========================
void logPackageAccess(String rfidCode, String accessType) {
  FirebaseJson json;
  json.set("userId", "unknown"); // Would be looked up from RFID
  json.set("type", "package_access");
  json.set("message", "Package access via RFID: " + accessType);
  json.set("createdAt", getCurrentTimestamp());
  json.set("metadata/rfidCode", rfidCode);
  json.set("metadata/accessType", accessType);
  json.set("metadata/deviceId", systemState.deviceId);
  
  String activityId = "activity_" + String(millis()) + "_" + rfidCode;
  Firebase.Firestore.createDocument(&fbdo, PROJECT_ID, "", "globalActivities/" + activityId, json.raw());
}

// ======================== DISPLAY MANAGEMENT ========================
void updateDisplay(String line1, String line2) {
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print(line1.substring(0, 16)); // Limit to 16 characters
  lcd.setCursor(0, 1);
  lcd.print(line2.substring(0, 16)); // Limit to 16 characters
}

void updateSystemDisplay() {
  static unsigned long lastDisplayUpdate = 0;
  
  if (millis() - lastDisplayUpdate > DISPLAY_UPDATE_INTERVAL) {
    if (systemState.currentSession == "") {
      // Show capacity status
      String line1 = "Cap: " + String((int)capacityData.percentage) + "%";
      String line2 = capacityData.status;
      updateDisplay(line1, line2);
    }
    
    lastDisplayUpdate = millis();
  }
}

// ======================== UTILITY FUNCTIONS ========================
String generateRandomRFID() {
  String rfid = "";
  for (int i = 0; i < 8; i++) {
    rfid += String(random(0, 16), HEX);
  }
  rfid.toUpperCase();
  return rfid;
}

String getCurrentTimestamp() {
  // In a real implementation, this would get actual timestamp
  return String(millis());
}

void sendHeartbeat() {
  FirebaseJson json;
  json.set("deviceId", systemState.deviceId);
  json.set("lastHeartbeat", getCurrentTimestamp());
  json.set("wifiConnected", systemState.wifiConnected);
  json.set("firebaseConnected", systemState.firebaseConnected);
  json.set("currentSession", systemState.currentSession);
  json.set("uptime", millis());
  
  Firebase.RTDB.setJSON(&fbdo, "/systemStatus/devices/" + systemState.deviceId, &json);
}

// ======================== ERROR HANDLING ========================
void handleFirebaseError(String operation) {
  Serial.println("Firebase Error in " + operation + ": " + fbdo.errorReason());
  
  if (fbdo.errorReason().indexOf("network") >= 0) {
    // Network error, try to reconnect
    systemState.firebaseConnected = false;
    delay(5000);
    initializeFirebase();
  }
}