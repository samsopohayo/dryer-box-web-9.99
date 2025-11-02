#include <Arduino.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>
#include <FirebaseESP32.h>
#include <Fuzzy.h>
#include <DHT.h>
#include <HX711.h>
#include <LiquidCrystal_I2C.h>
#include <ESP32Servo.h>
#include <time.h>
#include <SPIFFS.h>

// Firebase Configuration
#define FIREBASE_HOST "dryer-box-default-rtdb.asia-southeast1.firebasedatabase.app"
#define FIREBASE_AUTH "AIzaSyCkXIB8zbRrR27K2kmt_S39pCKLQT5Wzls"
#define WIFI_SSID "OPANG 4G"
#define WIFI_PASSWORD "opang9876"

// Pin Definitions 
#define DHT_PIN 4           // Sensor DHT
#define DOUT_PIN 14         // Sensor loadcell
#define SCK_PIN 12          // Sensor loadcell
#define TARE_SWITCH 32      // TARE button (LONG PRESS untuk restart)
#define START_SWITCH 33     // START button
#define INFO_SWITCH 35      // INFO button 
#define RESET_SWITCH 34     // RESET button 
#define DOOR_SWITCH 23      // Door switch
#define FANHEATPAN1 26      // Kipas heater panel 1
#define FANHEATCOL2 25      // Kipas heater colector 2
#define RELAY1_PIN 17       // Heater Hangat
#define RELAY2_PIN 5        // Heater Panas
#define MOSFET1_PIN 16      // Kipas exhaust
#define MOSFET2_PIN 15      // Steam pusher (reserved)
#define SERVO_PIN 18        // Exhaust valve
#define SDA_PIN 21          // I2C LCD SDA
#define SCL_PIN 22          // I2C LCD SCL


// System Configuration
#define DHTTYPE DHT22              // Tipe sensor suhu dan kelembaban 
#define RELAY_ON  LOW              // Pembalikan nilai relay
#define RELAY_OFF HIGH             // Pembalikan nilai relay
#define MAX_TEMP_LIMIT 80.0        // Proteksi suhu maksimal
#define INFO_DISPLAY_TIMEOUT 10000 // 10 detik timeout untuk info display
#define HUMIDITY_HYSTERESIS 2.0    // Hysteresis untuk menghindari oscillation

// Sensor Configuration
DHT dht(DHT_PIN, DHTTYPE);
HX711 scale;
LiquidCrystal_I2C lcd(0x27, 16, 2);  // 16x2 LCD
Servo exhaustServo;

// Firebase Objects
FirebaseData firebaseData;
FirebaseConfig config;
FirebaseAuth auth;

// Fuzzy Logic Setup
Fuzzy *fuzzy = new Fuzzy();

// Input Fuzzy Sets - Suhu
FuzzySet *dinginSuhu = new FuzzySet(0, 0, 50, 60);
FuzzySet *normalSuhu = new FuzzySet(50, 60, 60, 70);
FuzzySet *panasSuhu = new FuzzySet(60, 70, 100, 100);

// Input Fuzzy Sets - Kadar Air
FuzzySet *keringKadar = new FuzzySet(0, 0, 11, 50);
FuzzySet *normalKadar = new FuzzySet(11, 50, 50, 91);
FuzzySet *lembabKadar = new FuzzySet(50, 91, 100, 100);

// Input Fuzzy Sets - Humidity
FuzzySet *rendahHumidity = new FuzzySet(0, 0, 25, 35);
FuzzySet *idealHumidity = new FuzzySet(25, 35, 35, 45);
FuzzySet *tinggiHumidity = new FuzzySet(35, 45, 100, 100);

// Output Fuzzy Sets - Exhaust 
FuzzySet *tutupExhaust = new FuzzySet(0, 0, 30, 50);
FuzzySet *sedangExhaust = new FuzzySet(30, 50, 70, 90);
FuzzySet *bukaExhaust = new FuzzySet(70, 90, 100, 100);

// Output Fuzzy Sets - Pemanas
FuzzySet *matiPemanas = new FuzzySet(0, 0, 85, 127);
FuzzySet *hangatPemanas = new FuzzySet(85, 127, 127, 170);
FuzzySet *panasPemanas = new FuzzySet(127, 170, 255, 255);

// Output Fuzzy Sets - Kipas
FuzzySet *matiKipas = new FuzzySet(0, 0, 85, 127);
FuzzySet *lambatKipas = new FuzzySet(85, 127, 127, 170);
FuzzySet *cepatKipas = new FuzzySet(127, 170, 255, 255);

// Global Variables
unsigned long stableStartTime = 0;
unsigned long lastConfigCheck = 0;
float targetMoisture = 15.0;
float targetHumidityMin = 30.0;
float targetHumidityMax = 40.0;
float lastWeight = 0;
String processStatus = "Siap";
String pemanasStatus = "Mati";
String kipasStatus = "Mati";
String exhaustStatus = "Tutup";
String sessionId = "";
bool stabilizing = false;
bool systemRunning = false;
bool tempProtectionActive = false;
bool humidityControlActive = false;
int timezone = 7 * 3600;
int dst = 0;

// Input data sensor
float suhuSensor = 0;
float humiditySensor = 0;
float kadarAir = 0;
float beratAwal = 0;
float beratSekarang = 0;
bool doorOpen = false;

// Heater Fan Fuzzy Controller  
const int fanPanChannel  = 0;            // channel LEDC untuk fanPanChannel 
const int fanColChannel  = 1;            // channel LEDC untuk fanColChannel
const int pwmFreq = 20000;               // 20 kHz
const int pwmResolution = 8;             // 8-bit (0..255)

// Weather API variables
String weatherMain = "";
String weatherDesc = "";
float tempOutside = 0.0;
unsigned long lastWeatherUpdate = 0;
const unsigned long weatherInterval = 30UL * 60UL * 1000UL; // update tiap 30 menit

// Manual Control Variables
bool manualHeaterControl = false;
bool manualFanControl = false;
bool manualExhaustControl = false;
bool manualHeaterState = false;
bool manualFanState = false;
bool manualExhaustState = false;

// System States
enum SystemState {
  WAIT_TARE,
  WAIT_OBJECT,
  WAIT_START,
  RUNNING
};
SystemState currentState = WAIT_TARE;

// Error System
struct SystemError {
  String code;
  String message;
  bool active;
  unsigned long timestamp;
};

SystemError systemErrors[10];
int errorCount = 0;
bool hasActiveErrors = false;

// Info Display System
enum InfoDisplayMode {
  INFO_DEFAULT,
  INFO_SENSORS,
  INFO_WEIGHTS,
  INFO_WIFI,
  INFO_SYSTEM,
  INFO_HISTORY
};

InfoDisplayMode currentInfoMode = INFO_DEFAULT;
unsigned long infoDisplayStart = 0;
bool infoDisplayActive = false;

// SPIFFS Buffer for offline storage
struct DataPoint {
  String timestamp;
  float temperature;
  float humidity;
  float moisture;
  float weight;
  String status;
  String heaterStatus;
  String fanStatus;
  String exhaustStatus;
};

std::vector<DataPoint> offlineBuffer;
bool offlineMode = false;

// Button State Variables
struct ButtonState {
  bool currentPressed;
  bool lastPressed;
  unsigned long lastPressTime;
  unsigned long lastReleaseTime;
  int clickCount;
  bool waitingForDoubleClick;
};

ButtonState tareButton = {false, false, 0, 0, 0, false};
ButtonState startButton = {false, false, 0, 0, 0, false};
ButtonState infoButton = {false, false, 0, 0, 0, false};

// Declarations ======================================================================================================
void turnOffAllOutputs();
void updateDefaultDisplay();
void handleButtonInputs();
void handleTareButtonLongPress();
void handleStartButton();
void handleInfoButton();
void updateDoorStatus();
void checkTemperatureProtection();
void checkHumidityControl();
void performTare();
void startDryingProcess();
void restartSystemLongPress();
void readSensors();
void calculateMoistureContent();
void checkProcessCompletion();
void applyControls();
void applyManualControls();
void applyFuzzyLogicControls();
void applyHumidityExhaustControl();
void controlHeater(float output);
void controlFan(float output);
void controlExhaust(float output);
void controlExhaustHumidity(float humidity);
void controlHeaterManual(bool state);
void controlFanManual(bool state);
void controlExhaustManual(bool state);
void addError(String code, String message);
void clearError(String code);
void clearAllErrors();
void displayErrors();
void checkObjectWeight();
void runDryingProcess();
void cycleInfoDisplay();
void displayInfo();
void handleInfoDisplayTimeout();
void saveDataPoint();
void syncOfflineData();

// Utility ======================================================================================================
String getTimestamp() {
  time_t now = time(nullptr);
  struct tm* ptm = localtime(&now);
  char buffer[20];
  sprintf(buffer, "%02d/%02d/%04d %02d:%02d:%02d", 
          ptm->tm_mday, ptm->tm_mon + 1, ptm->tm_year + 1900,
          ptm->tm_hour, ptm->tm_min, ptm->tm_sec);
  return String(buffer);
}

String generateSessionId() {
  time_t now = time(nullptr);
  struct tm* ptm = localtime(&now);
  char buffer[16];
  sprintf(buffer, "%04d%02d%02d_%02d%02d%02d", 
          ptm->tm_year + 1900, ptm->tm_mon + 1, ptm->tm_mday,
          ptm->tm_hour, ptm->tm_min, ptm->tm_sec);
  return String(buffer);
}

// Set initial configuration: target moisture, humidity min, and humidity max ======================================================================================================
void loadConfigFromFirebase() {
  if (Firebase.getFloat(firebaseData, "/config/targetMoisture")) {
    targetMoisture = firebaseData.floatData();
  }
  if (Firebase.getFloat(firebaseData, "/config/targetHumidityMin")) {
    targetHumidityMin = firebaseData.floatData();
  }
  if (Firebase.getFloat(firebaseData, "/config/targetHumidityMax")) {
    targetHumidityMax = firebaseData.floatData();
  }

  Serial.println("📥 Config loaded from Firebase:");
  Serial.println("  Target Moisture    : " + String(targetMoisture));
  Serial.println("  Target Humidity Min: " + String(targetHumidityMin));
  Serial.println("  Target Humidity Max: " + String(targetHumidityMax));
}

// Update target values if changed in Firebase ======================================================================================================
void listenConfigUpdates() {
  if (Firebase.getFloat(firebaseData, "/config/targetMoisture")) {
    targetMoisture = firebaseData.floatData();
  }
  if (Firebase.getFloat(firebaseData, "/config/targetHumidityMin")) {
    targetHumidityMin = firebaseData.floatData();
  }
  if (Firebase.getFloat(firebaseData, "/config/targetHumidityMax")) {
    targetHumidityMax = firebaseData.floatData();
  }
}

// Setup Function ======================================================================================================
void setup() {
  Serial.begin(115200);
  Serial.println("\n🚀 SISTEM PENGERING ESP32 v3.3");
  Serial.println("===================================");
  Serial.println("Initializing enhanced system");
  
  // Initialize SPIFFS first
  if (!SPIFFS.begin(true)) {
    Serial.println("❌ SPIFFS initialization failed!");
  } else {
    Serial.println("✅ SPIFFS initialized successfully");
  }

  // set weather data function
  fetchWeatherData();

  // Set initial configuration: target moisture, humidity min, and humidity max
  loadConfigFromFirebase();
  
  // Initialize LCD first for status display
  lcd.init();
  lcd.backlight();
  displayBootMessage();
  Serial.println("✅ LCD 16x2 initialized with boot message");
  
  // Initialize all error entries
  for (int i = 0; i < 10; i++) {
    systemErrors[i].active = false;
  }
  
  // Initialize pins - UPDATED CONFIGURATION
  ledcSetup(fanPanChannel, pwmFreq, pwmResolution);   // Pin 26 - Kipas heater panel 1
  ledcAttachPin(FANHEATPAN1, fanPanChannel);
  ledcSetup(fanColChannel, pwmFreq, pwmResolution);   // Pin 25 - Kipas heater colector 2
  ledcAttachPin(FANHEATCOL2, fanColChannel);
  pinMode(TARE_SWITCH, INPUT_PULLUP);                 // Pin 32 - Long press untuk restart
  pinMode(START_SWITCH, INPUT_PULLUP);                // Pin 33 - Start button
  pinMode(DOOR_SWITCH, INPUT_PULLUP);                 // Pin 23 - Door switch
  pinMode(INFO_SWITCH, INPUT_PULLUP);                 // Pin 35 - Info button 
  pinMode(RESET_SWITCH, INPUT_PULLUP);                // pin 34 - Reset button
  pinMode(RELAY1_PIN, OUTPUT);                        // Pin 17 - Heater1
  pinMode(RELAY2_PIN, OUTPUT);                        // Pin 5  - Heater2
  pinMode(MOSFET1_PIN, OUTPUT);                       // Pin 16 - Kipas exhaust
  pinMode(MOSFET2_PIN, OUTPUT);                       // Pin 15 - Steam pusher (reserved)
  Serial.println("✅ GPIO pins configured ");
  
  // Test button pins
  testButtonPins();
  
  // Initialize all outputs to OFF
  turnOffAllOutputs();
  Serial.println("✅ All outputs set to OFF state");
  
  // Initialize DHT22 sensor
  dht.begin();
  delay(2000);
  testDHTSensor();
  
  // Initialize HX711 load cell
  scale.begin(DOUT_PIN, SCK_PIN);
  testLoadCell();
  
  // Initialize servo
  exhaustServo.attach(SERVO_PIN);
  exhaustServo.write(0);
  Serial.println("✅ Servo exhaust initialized - Position: CLOSED");
  
  // WiFi Connection
  connectWiFi();
  
  // Firebase setup
  setupFirebase();
  
  // Time setup
  configTime(timezone, dst, "pool.ntp.org", "time.nist.gov");
  Serial.println("✅ NTP time server configured");
  
  // Setup Fuzzy Logic
  setupFuzzyLogic();
  setupFuzzyRules();
  Serial.println("✅ Fuzzy logic system initialized");
  
  // Test all outputs
  Serial.println("🔧 Testing all outputs...");
  testAllOutputs();
  
  // Initial display
  updateDefaultDisplay();
  
  // Send system ready status to Firebase
  sendSystemStatus("ready");
  
  Serial.println("🚀 ENHANCED SYSTEM READY!");  
  Serial.println("💡 LONG PRESS TARE BUTTON TO RESTART SYSTEM");
  Serial.println("=====================================================");
}

// Loop Function ======================================================================================================
void loop() {
  // Handle all button inputs with long press detection
  handleButtonInputs();
  
  // Check manual control from Firebase
  checkManualControls();
  
  // Update door status
  updateDoorStatus();
  
  // Check temperature protection
  checkTemperatureProtection();

  // Check weather data
  if (millis() - lastWeatherUpdate > weatherInterval) {
    fetchWeatherData();
    lastWeatherUpdate = millis();
  }

  // Check humidity control
  checkHumidityControl();
  
  // Handle info display timeout
  handleInfoDisplayTimeout();
  
  // Main state machine
  switch (currentState) {
    case WAIT_TARE:
      break;
      
    case WAIT_OBJECT:
      checkObjectWeight();
      break;
      
    case WAIT_START:
      break;
      
    case RUNNING:
      if (systemRunning) {
        runDryingProcess();
        delay(15000);
      }
      break;
  }
  
  // Update display based on current mode
  updateDisplay();
  
  // Sync offline data if WiFi reconnected
  if (WiFi.status() == WL_CONNECTED && offlineMode) {
    syncOfflineData();
  }

  // Sync nilai target
  if (millis() - lastConfigCheck > 5000) {
    listenConfigUpdates();
    lastConfigCheck = millis();
  }
  
  delay(200); // Small delay for system stability
}

// HUMIDITY CONTROL FUNCTION ======================================================================================================!
void checkHumidityControl() {

  if (!systemRunning || tempProtectionActive || doorOpen) {
    humidityControlActive = false;
    return;
  }
  
  if (isnan(humiditySensor)) {
    humidityControlActive = false;
    return;
  }
  
  if (humiditySensor > (targetHumidityMax + HUMIDITY_HYSTERESIS)) {
    if (!humidityControlActive) {
      humidityControlActive = true;
      Serial.println("🌡  HUMIDITY CONTROL ACTIVATED!");
      Serial.println("   Current: " + String(humiditySensor, 1) + "% > Target: " + String(targetHumidityMax, 1) + "%");
    }
    
    controlExhaustHumidity(humiditySensor);
    
  } else if (humiditySensor < (targetHumidityMin - HUMIDITY_HYSTERESIS)) {
    if (humidityControlActive) {
      humidityControlActive = false;
      Serial.println("🌡  HUMIDITY CONTROL DEACTIVATED!");
      Serial.println("   Current: " + String(humiditySensor, 1) + "% < Target: " + String(targetHumidityMin, 1) + "%");
      
      digitalWrite(MOSFET1_PIN, LOW);
      exhaustServo.write(0);
      exhaustStatus = "Tutup";
    }
    
  } else if (humiditySensor >= targetHumidityMin && humiditySensor <= targetHumidityMax) {
    if (humidityControlActive) {
      humidityControlActive = false;
      Serial.println("🌡  HUMIDITY IN IDEAL RANGE!");
      Serial.println("   Current: " + String(humiditySensor, 1) + "% (Target: " + String(targetHumidityMin, 1) + "-" + String(targetHumidityMax, 1) + "%)");
      
      digitalWrite(MOSFET1_PIN, LOW);
      exhaustServo.write(0);
      exhaustStatus = "Tutup";
    }
  }
}

// Control Exhaust ======================================================================================================*
void controlExhaustHumidity(float humidity) {
  float excess = humidity - targetHumidityMax;
  
  if (excess <= 5.0) {
    // Kelebihan ringan (40-45%) - exhaust sedang
    digitalWrite(MOSFET1_PIN, HIGH);
    exhaustServo.write(45);
    exhaustStatus = "Sedang-Humid";
    Serial.println("Exhaust SEDANG - Excess humidity: " + String(excess, 1) + "%");
    
  } else if (excess <= 10.0) {
    // Kelebihan sedang (45-50%) - exhaust cepat
    digitalWrite(MOSFET1_PIN, HIGH);
    exhaustServo.write(70);
    exhaustStatus = "Cepat-Humid";
    Serial.println("Exhaust CEPAT - Excess humidity: " + String(excess, 1) + "%");
    
  } else {
    // Kelebihan tinggi (>50%) - exhaust maksimal
    digitalWrite(MOSFET1_PIN, HIGH);
    exhaustServo.write(90);
    exhaustStatus = "Maksimal-Humid";
    Serial.println("Exhaust MAKSIMAL - Excess humidity: " + String(excess, 1) + "%");
  }
}

// Enhaced Button Handling and Restart System With Long Pess On Tare ======================================================================================================
void handleButtonInputs() {
  handleTareButtonLongPress();
  handleStartButton();
  handleInfoButton();
}

void handleTareButtonLongPress() {
  static bool isPressed = false;
  static unsigned long pressStartTime = 0;

  bool currentReading = (digitalRead(TARE_SWITCH) == LOW);

  if (currentReading && !isPressed) {
    isPressed = true;
    pressStartTime = millis();
    Serial.println("🔘 TARE button pressed - monitoring long press...");
  }
  if (isPressed && currentReading) {
    unsigned long heldTime = millis() - pressStartTime;

    if (heldTime >= 3000) {  
      for (int i = 1; i <= 3; i++) {
        lcd.clear();
        lcd.setCursor(0, 0);
        lcd.print("Hold for restart");
        lcd.setCursor(0, 1);
        lcd.print("Count: " + String(i));
        Serial.println("⏱️ Hold count: " + String(i));
        delay(1000);
      }

      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("RESTARTING");
      lcd.setCursor(0, 1);
      lcd.print("SYSTEM...");
      Serial.println("🔁 Long press - Restart system!");
      delay(3000);

      restartSystemLongPress();
      isPressed = false;
    }
  }

  if (!currentReading && isPressed) {
    unsigned long heldTime = millis() - pressStartTime;
    isPressed = false;

    if (heldTime < 3000) {
      Serial.println("🔘 Short press - perform TARE");
      performTare();
    }
  }
}

void handleStartButton() {
  bool currentReading = (digitalRead(START_SWITCH) == LOW);
  unsigned long currentTime = millis();
  
  if (currentReading && !startButton.lastPressed && 
      (currentTime - startButton.lastPressTime) > 50) { // Debounce
    
    startButton.lastPressTime = currentTime;
    Serial.println("🔘 START button pressed");
    
    if (currentState == WAIT_START) {
      startDryingProcess();
    } else {
      Serial.println("   START ignored - system not ready");
      
      // Show status on LCD temporarily
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("START IGNORED");
      lcd.setCursor(0, 1);
      lcd.print("System not ready");
      delay(1500);
    }
  }
  
  startButton.lastPressed = currentReading;
}

void handleInfoButton() {
  bool currentReading = (digitalRead(INFO_SWITCH) == LOW);
  unsigned long currentTime = millis();
  
  if (currentReading && !infoButton.lastPressed && 
      (currentTime - infoButton.lastPressTime) > 50) { // Debounce
    
    infoButton.lastPressTime = currentTime;
    Serial.println("🔘 INFO button pressed");
    
    if (!hasActiveErrors) {
      cycleInfoDisplay();
    } else {
      Serial.println("   INFO ignored - system has active errors");
    }
  }
  
  infoButton.lastPressed = currentReading;
}

// ENHANCED LCD DISPLAY FUNCTIONS on 16x2 ======================================================================================================
void displayBootMessage() {
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("SISTEM PENGERING");
  lcd.setCursor(0, 1);
  lcd.print("ESP32 v3.3");
  delay(2000);
  
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("LongPress TARE t");
  lcd.setCursor(0, 1);
  lcd.print("restart system");
  delay(3000);
}

// Function Button Test ====================================================================================================== 
void testButtonPins() {
  Serial.println("\n🔧 TESTING BUTTON PINS...");
  
  Serial.print("   DOOR (Pin 23): ");
  Serial.println(digitalRead(DOOR_SWITCH) ? "HIGH (OK)" : "LOW");
  
  Serial.print("   TARE (Pin 32): ");
  Serial.println(digitalRead(TARE_SWITCH) ? "HIGH (OK)" : "LOW");
  
  Serial.print("   START (Pin 33): ");
  Serial.println(digitalRead(START_SWITCH) ? "HIGH (OK)" : "LOW");
  
  Serial.print("   INFO (Pin 13): ");
  Serial.println(digitalRead(INFO_SWITCH) ? "HIGH (OK)" : "LOW");
  
  Serial.println("✅ Button pin test completed");
  Serial.println("   All should show HIGH when not pressed (pull-up active)");
  Serial.println("💡 Remember: LONG PRESS TARE for system restart!");
}

// Function Update Display ======================================================================================================
void updateDisplay() {
  if (hasActiveErrors) {
    displayErrors();
  } else if (infoDisplayActive) {
    displayInfo();
  } else {
    updateDefaultDisplay();
  }
}

// Function Default Display ======================================================================================================
void updateDefaultDisplay() {
  lcd.clear();
  
  switch (currentState) {
    case WAIT_TARE:
      lcd.setCursor(0, 0);
      lcd.print("Tekan TARE untuk");
      lcd.setCursor(0, 1);
      lcd.print("reset timbangan");
      break;

    case WAIT_OBJECT:{
      lcd.setCursor(0, 0);
      lcd.print("Masukkan objek");
      lcd.setCursor(0, 1);

      float currentWeight = scale.get_units(10);
      if (currentWeight < 0 || currentWeight < 5.0) {
        lcd.print("Timbangan kosong ");
      } 
      else {
        lcd.print("Berat:" + String(currentWeight, 0) + "g    ");
      }
      break;
    }

    case WAIT_START:
      lcd.setCursor(0, 0);
      lcd.print("Awal:" + String(beratAwal, 0) + "g      ");
      lcd.setCursor(0, 1);
      lcd.print("Tekan START");
      break;
      
    case RUNNING:
      lcd.setCursor(0, 0);
      lcd.print("T:" + String(suhuSensor, 0) + " H:" + String(humiditySensor, 0) + " KA:" + String(kadarAir, 0));
      
      lcd.setCursor(0, 1);
      String statusLine = "";
      
      if (tempProtectionActive) {
        statusLine = "TEMP PROTECTION";
      } else if (doorOpen) {
        statusLine = "DOOR OPEN-PAUSED";
      } else {
        statusLine = "STATUS: " + processStatus;
      }
      
      // Pad or truncate to 16 characters
      while (statusLine.length() < 16) statusLine += " ";
      if (statusLine.length() > 16) statusLine = statusLine.substring(0, 16);
      
      lcd.print(statusLine);
      break;
  }
}

// Function Cycle Display ======================================================================================================
void cycleInfoDisplay() {
  if (!infoDisplayActive) {
    infoDisplayActive = true;
    currentInfoMode = INFO_SENSORS;
    infoDisplayStart = millis();
  } else {
    // Cycle to next info mode
    switch (currentInfoMode) {
      case INFO_SENSORS: currentInfoMode = INFO_WEIGHTS; break;
      case INFO_WEIGHTS: currentInfoMode = INFO_WIFI; break;
      case INFO_WIFI: currentInfoMode = INFO_SYSTEM; break;
      case INFO_SYSTEM: currentInfoMode = INFO_HISTORY; break;
      case INFO_HISTORY: currentInfoMode = INFO_SENSORS; break;
      default: currentInfoMode = INFO_SENSORS; break;
    }
    infoDisplayStart = millis();
  }
  
  Serial.println("ℹ  Info display mode: " + String(currentInfoMode));
}

// Function LCD Display ======================================================================================================
void displayInfo() {
  lcd.clear();

  switch (currentInfoMode) {
    case INFO_SENSORS: {
      lcd.setCursor(0, 0);
      lcd.print("== SENSORS ==");
      lcd.setCursor(0, 1);
      String sensorStatus = "DHT:";
      sensorStatus += (isnan(dht.readTemperature()) ? "ERR" : "OK");
      sensorStatus += " HX711:";
      sensorStatus += (scale.is_ready() ? "OK" : "ERR");
      lcd.print(sensorStatus);
      break;
    }

    case INFO_WEIGHTS: {
      lcd.setCursor(0, 0);
      lcd.print("=== WEIGHTS ===");
      lcd.setCursor(0, 1);
      lcd.print("A:" + String(beratAwal, 0) + "g S:" + String(beratSekarang, 0) + "g");
      break;
    }

    case INFO_WIFI: {
      lcd.setCursor(0, 0);
      lcd.print("=== NETWORK ===");
      lcd.setCursor(0, 1);
      if (WiFi.status() == WL_CONNECTED) {
        lcd.print("WiFi:" + String(WiFi.RSSI()) + "dBm OK");
      } else {
        lcd.print("WiFi: OFFLINE");
      }
      break;
    }

    case INFO_SYSTEM: {
      lcd.setCursor(0, 0);
      lcd.print("== HUMIDITY ==");
      lcd.setCursor(0, 1);
      String humidStatus = String(humiditySensor, 0) + "% ";
      if (humidityControlActive) {
        humidStatus += "CTRL-ON";
      } else if (humiditySensor >= targetHumidityMin && humiditySensor <= targetHumidityMax) {
        humidStatus += "IDEAL";
      } else {
        humidStatus += "OK";
      }
      lcd.print(humidStatus);
      break;
    }

    case INFO_HISTORY: {
      lcd.setCursor(0, 0);
      lcd.print("== STATUS ==");
      lcd.setCursor(0, 1);
      String statusLine = "";
      statusLine += " Htr:" + pemanasStatus.substring(0, 1);
      statusLine += " Fan:" + kipasStatus.substring(0, 1);
      statusLine += " Exh:" + exhaustStatus.substring(0, 1);
      statusLine += " Hum:" + String(humidityControlActive ? "!" : "OK");
      lcd.print(statusLine);
      break;
    }
  }
}

// Function Timeout Info ======================================================================================================
void handleInfoDisplayTimeout() {
  if (infoDisplayActive && (millis() - infoDisplayStart > INFO_DISPLAY_TIMEOUT)) {
    infoDisplayActive = false;
    currentInfoMode = INFO_DEFAULT;
    Serial.println("ℹ  Info display timeout - returning to default");
  }
}

// Function Error Display ======================================================================================================
void displayErrors() {
  // Find first active error and display it
  for (int i = 0; i < errorCount; i++) {
    if (systemErrors[i].active) {
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("!!! ERROR !!!");
      lcd.setCursor(0, 1);
      
      String errorMsg = systemErrors[i].code;
      // Truncate long error codes for 16x2 display
      if (errorMsg.length() > 16) {
        errorMsg = errorMsg.substring(0, 16);
      }
      while (errorMsg.length() < 16) errorMsg += " ";
      
      lcd.print(errorMsg);
      break;
    }
  }
}

// Restart Function - Triggered by Long Press ======================================================================================================!!!
void restartSystemLongPress() {
  Serial.println("\n=== LONG PRESS RESTART INITIATED ===");
  Serial.println("Restart triggered by Long Press TARE button");
  
  // Show restart progress on LCD
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("RESTARTING...");
  lcd.setCursor(0, 1);
  lcd.print("Please wait...");
  
  // Stop current process immediately
  systemRunning = false;
  turnOffAllOutputs();
  delay(1000);
  
  // Reset all states
  currentState = WAIT_TARE;
  processStatus = "Ready";
  beratAwal = 0;
  beratSekarang = 0;
  kadarAir = 0;
  humiditySensor = 0;
  sessionId = "";
  tempProtectionActive = false;
  humidityControlActive = false;
  
  // Clear manual controls
  manualHeaterControl = false;
  manualFanControl = false;
  manualExhaustControl = false;
  
  // Clear errors
  clearAllErrors();
  
  // Reset display modes
  currentInfoMode = INFO_DEFAULT;
  infoDisplayActive = false;
  
  // Reset button states
  tareButton = {false, false, 0, 0, 0, false};
  startButton = {false, false, 0, 0, 0, false};
  infoButton = {false, false, 0, 0, 0, false};
  
  Serial.println("✅ System restart completed - Ready for new process");
  
  // Send restart status to Firebase
  sendSystemStatus("restarted system");
  
  // Show completion message
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("RESTART SUCCESS");
  lcd.setCursor(0, 1);
  lcd.print("System Ready!");
  delay(2000);
  
  // Return to default display
  updateDefaultDisplay();
  
  Serial.println("🎉 LONG PRESS restart sequence completed!");
  Serial.println("💡 LONG PRESS TARE again anytime to restart");
  Serial.println("================================================");
}

// Function Tare ======================================================================================================
void performTare() {
  Serial.println("\n⚖  === PERFORMING TARE OPERATION ===");
  
  clearError("SCALE_NOT_READY");
  clearError("TARE_FAILED");
  
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Melakukan TARE..");
  lcd.setCursor(0, 1);
  lcd.print("Mohon tunggu...");
  
  if (!scale.is_ready()) {
    addError("SCALE_NOT_READY", "Scale not ready!");
    Serial.println("❌ Scale not ready for tare!");
    return;
  }
  
  Serial.println("🔄 Taring scale...");
  scale.tare(20);
  delay(2000);
  
  // Verify tare operation
  float testReading = scale.get_units(3);
  if (abs(testReading) < 5.0) {
    Serial.println("✅ Tare completed successfully");
    Serial.println("   Scale reading after tare: " + String(testReading, 2) + " gram");
    
    // Reset system to wait for object
    currentState = WAIT_OBJECT;
    processStatus = "Siap";
    
    // Show success message
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("TARE SUCCESS!");
    lcd.setCursor(0, 1);
    lcd.print("Masukkan objek");
    delay(2000);
    
    // Send tare status to Firebase
    sendSystemStatus("tare_completed");
    
  } else {
    addError("TARE_FAILED", "Tare verification failed: " + String(testReading, 2) + "g");
    Serial.println("⚠  Tare verification failed");
  }
}

// Function Check Object ======================================================================================================
void checkObjectWeight() {
  if (!scale.is_ready()) {
    addError("SCALE_NOT_READY", "Scale not ready");
    return;
  }

  clearError("SCALE_NOT_READY");
  float weight = scale.get_units(5);  // ambil bacaan rata-rata 5 kali

  if (weight > 5.0) {
    if (abs(weight - lastWeight) <= 7.0) {
      if (!stabilizing) {
        stabilizing = true;
        stableStartTime = millis();
      } else {
        if (millis() - stableStartTime >= 4000) {
          beratAwal = weight;
          Serial.println("📦 Object detected");
          Serial.println("📦 Initial weight: " + String(beratAwal, 1) + " gram");

          currentState = WAIT_START;
          processStatus = "Siap";

          lcd.clear();
          lcd.setCursor(0, 0);
          lcd.print("OBJEK TERDETEKSI");
          lcd.setCursor(0, 1);
          lcd.print("Berat:" + String(beratAwal, 0) + "g");
          delay(2000);

          Firebase.setFloat(firebaseData, "/sensor/berat_awal", beratAwal);

          stabilizing = false;
        }
      }
    } else {
      stabilizing = false;
    }
  } else {
    stabilizing = false;
  }
  lastWeight = weight;
}

// Function Start ======================================================================================================
void startDryingProcess() {
  Serial.println("\n🚀 === STARTING DRYING PROCESS ===");
  
  // Generate new session ID
  sessionId = generateSessionId();
  Serial.println("   Session ID: " + sessionId);
  Serial.println("   Initial weight: " + String(beratAwal, 1) + " gram");
  Serial.println("   Humidity control target: " + String(targetHumidityMin, 1) + "-" + String(targetHumidityMax, 1) + "%");
  
  systemRunning = true;
  currentState = RUNNING;
  processStatus = "Berjalan";
  
  // Show start message on LCD
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("PROSES DIMULAI!");
  lcd.setCursor(0, 1);
  lcd.print("Humid:" + String(targetHumidityMin, 0) + "-" + String(targetHumidityMax, 0) + "%");
  delay(2000);
  
  // Send start status to Firebase
  sendSessionStart();
  
  Serial.println("✅ Drying process started successfully!");
}

// Function Drying ======================================================================================================
void runDryingProcess() {
  Serial.println("\n📊 === READING SENSORS ===");
  
  readSensors();
  
  calculateMoistureContent();
  
  checkProcessCompletion();
  
  if (processStatus == "Berjalan" && !tempProtectionActive && !doorOpen) {
    applyControls();
  } else {
    turnOffAllOutputs();
  }
  
  saveDataPoint();
  
  sendOrBufferData();
  
  Serial.println("✅ Process cycle completed");
  Serial.println("=====================================");
}

// Function Read Sensor DHT22 ======================================================================================================
void readSensors() {
  // Read DHT22 - Temperature dan Humidity
  suhuSensor = dht.readTemperature();
  humiditySensor = dht.readHumidity(); 
  
  if (isnan(suhuSensor)) {
    addError("DHT22_TEMP_FAILED", "Temperature sensor error");
    suhuSensor = 0;
  } else {
    clearError("DHT22_TEMP_FAILED");
    Serial.println("✅ DHT22 temperature: " + String(suhuSensor, 1) + "°C");
  }
  
  if (isnan(humiditySensor)) {
    addError("DHT22_HUMID_FAILED", "Humidity sensor error");
    humiditySensor = 0;
  } else {
    clearError("DHT22_HUMID_FAILED");
    Serial.println("✅ DHT22 humidity: " + String(humiditySensor, 1) + "%");
    
    // Log humidity status
    if (humiditySensor > targetHumidityMax) {
      Serial.println("   ⚠  Humidity TINGGI - Exhaust akan aktif");
    } else if (humiditySensor < targetHumidityMin) {
      Serial.println("   ⚠  Humidity RENDAH - Exhaust akan mati");
    } else {
      Serial.println("   ✅ Humidity IDEAL - dalam range target");
    }
  }
  
  // Read HX711
  if (!scale.is_ready()) {
    addError("SCALE_NOT_READY", "Load cell not ready");
    beratSekarang = beratAwal; // Keep last known weight
  } else {
    clearError("SCALE_NOT_READY");
    beratSekarang = scale.get_units(5);
    Serial.println("✅ HX711 weight: " + String(beratSekarang, 1) + " gram");
  }
}

// Function Calculation Moisture ======================================================================================================
void calculateMoistureContent() {
  if (beratSekarang <= 0 || beratAwal <= 0) {
    kadarAir = 0;
  } else {
    kadarAir = (beratSekarang / beratAwal) * 100;
    if (kadarAir > 100) kadarAir = 100;
    if (kadarAir < 0) kadarAir = 0;
  }
  Serial.println("📊 Moisture content: " + String(kadarAir, 1) + "%");
}

// Function Check Completion ======================================================================================================
void checkProcessCompletion() {
  if (kadarAir <= targetMoisture && processStatus == "Berjalan") {
    Serial.println("🎉 Drying process completed!");
    Serial.println("   Target moisture reached: " + String(kadarAir, 1) + "%");
    
    processStatus = "Selesai";
    systemRunning = false;
    humidityControlActive = false;
    turnOffAllOutputs();
    
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("PROSES SELESAI!");
    lcd.setCursor(0, 1);
    lcd.print("KA:" + String(kadarAir, 0) + "% H:" + String(humiditySensor, 0) + "%");
    
    sendSessionEnd();
  }
}

// Function Priority ======================================================================================================
void applyControls() {
  // Priority 1: Manual control 
  if (manualHeaterControl || manualFanControl || manualExhaustControl) {
    applyManualControls();
    return;
  }
  
  // Priority 2: Humidity-based exhaust control
  applyHumidityExhaustControl();
  
  // Priority 3: Fuzzy logic control untuk heater dan fan
  applyFuzzyLogicControls();
}

// Function Exhaust by Fuzzy ======================================================================================================!!!
void applyHumidityExhaustControl() {
  Serial.println("\n🌡  === HUMIDITY-BASED EXHAUST CONTROL ===");
  
  if (isnan(humiditySensor)) {
    Serial.println("   Humidity sensor error - using fuzzy logic for exhaust");
    return;
  }
  
  Serial.println("   Current humidity: " + String(humiditySensor, 1) + "%");
  Serial.println("   Target range: " + String(targetHumidityMin, 1) + "-" + String(targetHumidityMax, 1) + "%");
  
  if (humiditySensor > targetHumidityMax) {
    float excess = humiditySensor - targetHumidityMax;
    
    if (excess <= 5.0) {
      // Kelebihan ringan (40-45%) - exhaust pelan
      digitalWrite(MOSFET1_PIN, HIGH);
      exhaustServo.write(45);
      exhaustStatus = "Sedang-RH";
      Serial.println("   🌬  Exhaust PELAN - Excess: " + String(excess, 1) + "%");
      
    } else if (excess <= 15.0) {
      // Kelebihan sedang (45-55%) - exhaust sedang
      digitalWrite(MOSFET1_PIN, HIGH);
      exhaustServo.write(70);
      exhaustStatus = "Cepat-RH";
      Serial.println("   🌬  Exhaust SEDANG - Excess: " + String(excess, 1) + "%");
      
    } else {
      // Kelebihan tinggi (>55%) - exhaust cepat
      digitalWrite(MOSFET1_PIN, HIGH);
      exhaustServo.write(90);
      exhaustStatus = "Max-RH";
      Serial.println("   🌬  Exhaust CEPAT - Excess: " + String(excess, 1) + "%");
    }
    
  } else if (humiditySensor < targetHumidityMin) {
    digitalWrite(MOSFET1_PIN, LOW);
    exhaustServo.write(0);
    exhaustStatus = "Tutup-RH";
    Serial.println("   🌬  Exhaust TUTUP - Humidity terlalu rendah");
    
  } else {
    digitalWrite(MOSFET1_PIN, LOW);
    exhaustServo.write(10);
    exhaustStatus = "Ideal-RH";
    Serial.println("   🌬  Exhaust IDEAL - Humidity dalam range");
  }
}

// Function Manual All Control ======================================================================================================
void applyManualControls() {
  Serial.println("\n⚙  === MANUAL CONTROL MODE ===");
  
  if (manualHeaterControl) {
    controlHeaterManual(manualHeaterState);
    Serial.println("   Manual exhaust override - heater fuzzy control disabled");
  }
  
  if (manualFanControl) {
    controlFanManual(manualFanState);
    Serial.println("   Manual exhaust override - fan fuzzy control disabled");
  }
  
  if (manualExhaustControl) {
    controlExhaustManual(manualExhaustState);
    Serial.println("   Manual exhaust override - humidity fuzzy control disabled");
  }
}

// Function Fuzzy All Control ======================================================================================================
void applyFuzzyLogicControls() {
  Serial.println("\n🧠 === FUZZY LOGIC CONTROL MODE ===");
  
  fuzzy->setInput(1, suhuSensor);
  fuzzy->setInput(2, kadarAir);
  fuzzy->setInput(3, humiditySensor);
  
  fuzzy->fuzzify();
  
  float outputPemanas = fuzzy->defuzzify(1);
  float outputKipas = fuzzy->defuzzify(2);
  
  Serial.println("   Input - Suhu: " + String(suhuSensor, 1) + "°C");
  Serial.println("   Input - Kadar Air: " + String(kadarAir, 1) + "%");
  Serial.println("   Input - Humidity: " + String(humiditySensor, 1) + "%");
  Serial.println("   Output - Pemanas: " + String(outputPemanas, 1));
  Serial.println("   Output - Kipas: " + String(outputKipas, 1));
  
  if (!manualHeaterControl) controlHeater(outputPemanas);
  if (!manualFanControl) controlFan(outputKipas);
}


// CONTROL FAN ======================================================================================================
// Function Fan Fuzzy Control ======================================================================================================
void controlFan(float output) {
  int duty = constrain((int)round(output), 0, 255);
  int persen = map(duty, 0, 255, 0, 100);  // konversi ke 0–100%

  ledcWrite(fanPanChannel, duty);

  bool kondisiCuaca = (weatherMain == "Clear");   // hanya aktif saat kondisi cuaca cerah
  time_t now = time(nullptr);
  struct tm* ptm = localtime(&now);
  int jamSekarang = ptm->tm_hour;   // mengambil data jam lokal dari NTP


  if (duty == 0) {
    kipasStatus = "Mati";
  } else if (kondisiCuaca && jamSekarang < 17) {
    ledcWrite(fanColChannel, duty);   // kolektor ikut duty
    kipasStatus = "Pan & Col aktif (pwr: " + String(persen) + "%)";
  } else {
    ledcWrite(fanColChannel, 0);      // kolektor mati
    kipasStatus = "Hanya Pan (pwr: " + String(persen) + "%)";
  }

  Serial.println("💨 Fan status: " + kipasStatus);
}

// Function Fan Manual Control ======================================================================================================
void controlFanManual(bool state) {
  if (state) {
    ledcWrite(fanPanChannel, 255);
    ledcWrite(fanColChannel, 255);
    kipasStatus = "Manual ON";
  } else {
    ledcWrite(fanPanChannel, 0);
    ledcWrite(fanColChannel, 0);
    kipasStatus = "Manual OFF";
  }
  Serial.println("💨 Fan Manual: " + kipasStatus);
}


//CONTROL HEATER ======================================================================================================
// Function Heater Fuzzy Control ======================================================================================================
void controlHeater(float output) {
  if (output <= 84) {
    digitalWrite(RELAY1_PIN, RELAY_OFF);
    digitalWrite(RELAY2_PIN, RELAY_OFF);
    pemanasStatus = "Mati";
  } else if (output >= 85 && output <= 126) {
    digitalWrite(RELAY1_PIN, RELAY_ON);
    digitalWrite(RELAY2_PIN, RELAY_OFF);
    pemanasStatus = "Hangat";
  } else {
    digitalWrite(RELAY1_PIN, RELAY_ON);
    digitalWrite(RELAY2_PIN, RELAY_ON);
    pemanasStatus = "Panas";
  }
  Serial.println("🔥 Heater: " + pemanasStatus);
}

// Function Heater Manual Control ======================================================================================================
void controlHeaterManual(bool state) {
  if (state) {
    digitalWrite(RELAY1_PIN, RELAY_ON);
    digitalWrite(RELAY2_PIN, RELAY_ON);
    pemanasStatus = "Manual ON";
  } else {
    digitalWrite(RELAY1_PIN, RELAY_OFF);
    digitalWrite(RELAY2_PIN, RELAY_OFF);
    pemanasStatus = "Manual OFF";
  }
  Serial.println("🔥 Heater Manual: " + pemanasStatus);
}

// Control Exhaust ======================================================================================================
// Function Exhaust Fuzzy Control ======================================================================================================
void controlExhaust(float output) {
  if (output <= 30) {
    digitalWrite(MOSFET1_PIN, LOW);
    exhaustServo.write(0);
    exhaustStatus = "Tutup";
  } else if (output >= 30 && output <= 70) {
    digitalWrite(MOSFET1_PIN, HIGH);
    exhaustServo.write(45);
    exhaustStatus = "Sedang";
  } else {
    digitalWrite(MOSFET1_PIN, HIGH);
    exhaustServo.write(90);
    exhaustStatus = "Buka";
  }
  Serial.println("🌬  Exhaust: " + exhaustStatus);
}

// Function Exhaust Manual Control ======================================================================================================
void controlExhaustManual(bool state) {
  if (state) {
    digitalWrite(MOSFET1_PIN, HIGH);
    exhaustServo.write(90);
    exhaustStatus = "Manual ON";
  } else {
    digitalWrite(MOSFET1_PIN, LOW);
    exhaustServo.write(0);
    exhaustStatus = "Manual OFF";
  }
  Serial.println("🌬  Exhaust Manual: " + exhaustStatus);
}

// Control Door ======================================================================================================
void updateDoorStatus() {
  bool currentDoorState = (digitalRead(DOOR_SWITCH) == HIGH);
  
  if (currentDoorState != doorOpen) {
    doorOpen = currentDoorState;
    
    if (doorOpen && systemRunning) {
      Serial.println("🚪 Door opened - Process PAUSED for safety");
      processStatus = "Paused";
      humidityControlActive = false;
      turnOffAllOutputs();
      
      // Show door open warning on LCD
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("!PINTU TERBUKA!");
      lcd.setCursor(0, 1);
      lcd.print("Proses di-pause");
      
    } else if (!doorOpen && systemRunning && processStatus == "Paused") {
      Serial.println("🚪 Door closed - Process RESUMED");
      processStatus = "Berjalan";
    }
  }
}

// Control Temperature ======================================================================================================
void checkTemperatureProtection() {
  if (suhuSensor > MAX_TEMP_LIMIT) {
    if (!tempProtectionActive) {
      tempProtectionActive = true;
      humidityControlActive = false;
      addError("TEMP_PROTECTION", "Temperature too high: " + String(suhuSensor, 1) + "°C");
      Serial.println("🚨 TEMPERATURE PROTECTION ACTIVATED!");
      
      // Emergency cooling
      digitalWrite(RELAY1_PIN, RELAY_OFF);
      digitalWrite(RELAY2_PIN, RELAY_OFF);
      digitalWrite(MOSFET1_PIN, HIGH);
      digitalWrite(MOSFET2_PIN, HIGH);
      ledcWrite(fanPanChannel, 255);
      ledcWrite(fanColChannel, 255);
      exhaustServo.write(90);
      
      pemanasStatus = "PROTECTION";
      kipasStatus = "COOLING";
      exhaustStatus = "COOLING";
      
      // Show protection warning on LCD
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("OVER HEAT !!!");
      lcd.setCursor(0, 1);
      lcd.print("T:" + String(suhuSensor, 0) + "C>>" + String(MAX_TEMP_LIMIT, 0) + "C");
    }
  } else if (tempProtectionActive && suhuSensor < (MAX_TEMP_LIMIT - 5)) {
    tempProtectionActive = false;
    clearError("TEMP_PROTECTION");
    Serial.println("✅ Temperature protection deactivated - Normal operation resumed");
  }
}

// Control All Outputs Off ======================================================================================================
void turnOffAllOutputs() {
  digitalWrite(RELAY1_PIN, RELAY_OFF);
  digitalWrite(RELAY2_PIN, RELAY_OFF);
  digitalWrite(MOSFET1_PIN, LOW);
  digitalWrite(MOSFET2_PIN, LOW);
  ledcWrite(fanPanChannel, 0);
  ledcWrite(fanColChannel, 0);
  exhaustServo.write(0);
  
  if (!tempProtectionActive) {
    pemanasStatus = "Mati";
    kipasStatus = "Mati";
    exhaustStatus = "Tutup";
  }
}

// SPIFFS and Offline Functions ======================================================================================================
void saveDataPoint() {
  DataPoint dp;
  dp.timestamp = getTimestamp();
  dp.temperature = suhuSensor;
  dp.humidity = humiditySensor;
  dp.moisture = kadarAir;
  dp.weight = beratSekarang;
  dp.status = processStatus;
  dp.heaterStatus = pemanasStatus;
  dp.fanStatus = kipasStatus;
  dp.exhaustStatus = exhaustStatus;
  
  if (WiFi.status() != WL_CONNECTED) {
    // Save to offline buffer
    offlineBuffer.push_back(dp);
    offlineMode = true;
    
    // Limit buffer size
    if (offlineBuffer.size() > 100) {
      offlineBuffer.erase(offlineBuffer.begin());
    }
    
    Serial.println("💾 Data saved to offline buffer");
  }
}

// Sync Offline Data Function ======================================================================================================
void syncOfflineData() {
  if (offlineBuffer.empty()) return;
  
  Serial.println("🔄 Syncing offline data to Firebase...");
  
  for (auto& dp : offlineBuffer) {
    String path = "/sessions/" + sessionId + "/data/" + String(millis());
    
    // Create FirebaseJson object
    FirebaseJson json;
    json.set("timestamp", dp.timestamp);
    json.set("temperature", dp.temperature);
    json.set("humidity", dp.humidity);
    json.set("moisture", dp.moisture);
    json.set("weight", dp.weight);
    json.set("status", dp.status);
    json.set("heater", dp.heaterStatus);
    json.set("fan", dp.fanStatus);
    json.set("exhaust", dp.exhaustStatus);
    
    if (Firebase.setJSON(firebaseData, path, json)) {
      Serial.println("✅ Offline data synced: " + dp.timestamp);
    } else {
      Serial.println("❌ Failed to sync: " + dp.timestamp);
      break; // Stop if sync fails
    }
    
    delay(100); // Small delay between uploads
  }
  
  offlineBuffer.clear();
  offlineMode = false;
  Serial.println("🎉 All offline data synced successfully!");
}

// Firebase Functions ======================================================================================================
void connectWiFi() {
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Connecting WiFi..");
  lcd.setCursor(0, 1);
  lcd.print("SSID: OPANG 4G");
  
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  Serial.print("🌐 Connecting to WiFi");
  
  int attempts = 0;
  while (WiFi.status() != WL_CONNECTED && attempts < 30) {
    delay(500);
    Serial.print(".");
    attempts++;
    
    // Show connecting animation on LCD
    lcd.setCursor(14 + (attempts % 3), 0);
    lcd.print(".");
  }
  
  if (WiFi.status() == WL_CONNECTED) {
    Serial.println();
    Serial.println("✅ WiFi connected successfully!");
    Serial.println("   IP: " + WiFi.localIP().toString());
    Serial.println("   RSSI: " + String(WiFi.RSSI()) + " dBm");
    clearError("WIFI_FAILED");
    
    // Show success on LCD
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("WiFi Connected!");
    lcd.setCursor(0, 1);
    lcd.print("IP:" + WiFi.localIP().toString().substring(0, 12));
    delay(2000);
    
  } else {
    Serial.println();
    addError("WIFI_FAILED", "WiFi connection failed");
    offlineMode = true;
    
    // Show failure on LCD
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("WiFi Failed!");
    lcd.setCursor(0, 1);
    lcd.print("Offline Mode");
    delay(2000);
  }
}

// Function Setup Firebase ======================================================================================================
void setupFirebase() {
  config.host = FIREBASE_HOST;
  config.signer.tokens.legacy_token = FIREBASE_AUTH;
  Firebase.begin(&config, &auth);
  Firebase.reconnectWiFi(true);
  
  if (WiFi.status() == WL_CONNECTED) {
    if (Firebase.setString(firebaseData, "/system/status", "initializing")) {
      Serial.println("✅ Firebase connected successfully!");
      clearError("FIREBASE_FAILED");
    } else {
      addError("FIREBASE_FAILED", "Firebase connection failed");
      Serial.println("❌ Firebase connection ERROR!");
    }
  }
}

// setup OpenWeatherMap function ======================================================================================================
void fetchWeatherData() {
  if (WiFi.status() == WL_CONNECTED) {
    HTTPClient http;
    String url = "http://api.openweathermap.org/data/2.5/weather?q=Yogyakarta,id&appid=API_KEY&units=metric";
    http.begin(url);

    int httpCode = http.GET();
    if (httpCode == 200) {
      String payload = http.getString();
      
      DynamicJsonDocument doc(1024);
      DeserializationError error = deserializeJson(doc, payload);
      if (!error) {
        weatherMain = doc["weather"][0]["main"].as<String>();   // "Clear", "Clouds", "Rain"
        weatherDesc  = doc["weather"][0]["description"].as<String>();
        tempOutside  = doc["main"]["temp"].as<float>();

        Serial.println("☀️ Cuaca: " + weatherMain + " (" + weatherDesc + ")");
        Serial.println("🌡️ Suhu luar: " + String(tempOutside));
      }
    } else {
      Serial.println("❌ Gagal ambil data cuaca: " + String(httpCode));
    }
    http.end();
  }
}

// ======================================================================================================
void checkManualControls() {
  if (WiFi.status() != WL_CONNECTED) return;
  
  // Check manual heater control
  if (Firebase.getBool(firebaseData, "/control/manual_heater_enable")) {
    manualHeaterControl = firebaseData.boolData();
  }
  
  if (Firebase.getBool(firebaseData, "/control/manual_heater_state")) {
    manualHeaterState = firebaseData.boolData();
  }
  
  if (Firebase.getBool(firebaseData, "/control/manual_fan_enable")) {
    manualFanControl = firebaseData.boolData();
  }
  
  if (Firebase.getBool(firebaseData, "/control/manual_fan_state")) {
    manualFanState = firebaseData.boolData();
  }
  
  if (Firebase.getBool(firebaseData, "/control/manual_exhaust_enable")) {
    manualExhaustControl = firebaseData.boolData();
  }
  
  if (Firebase.getBool(firebaseData, "/control/manual_exhaust_state")) {
    manualExhaustState = firebaseData.boolData();
  }
}

// ======================================================================================================
void sendOrBufferData() {
  if (WiFi.status() == WL_CONNECTED && !offlineMode) {
    if (millis() - lastWeatherUpdate > weatherInterval) {
      fetchWeatherData();
      lastWeatherUpdate = millis();
    }
    sendToFirebase();
  } else {
    Serial.println("📤 Data buffered offline (WiFi unavailable)");
  }
}

// ======================================================================================================
void sendToFirebase() {
  Serial.println("\n📤 === SENDING DATA TO FIREBASE ===");
  
  bool allSuccess = true;

  // Send weather data
  if (!Firebase.setString(firebaseData, "/weather/main", weatherMain)) allSuccess = false;
  if (!Firebase.setString(firebaseData, "/weather/desc", weatherDesc)) allSuccess = false;
  if (!Firebase.setFloat(firebaseData, "/weather/temp", tempOutside)) allSuccess = false;  
  
  // Send sensor data
  if (!Firebase.setFloat(firebaseData, "/sensor/suhu", suhuSensor)) allSuccess = false;
  if (!Firebase.setFloat(firebaseData, "/sensor/humidity", humiditySensor)) allSuccess = false;
  if (!Firebase.setFloat(firebaseData, "/sensor/kelembaban", kadarAir)) allSuccess = false;
  if (!Firebase.setFloat(firebaseData, "/sensor/berat", beratSekarang)) allSuccess = false;
  
  // Send status data 
  if (!Firebase.setString(firebaseData, "/status/pengeringan", processStatus)) allSuccess = false;
  if (!Firebase.setString(firebaseData, "/status/pemanas", pemanasStatus)) allSuccess = false;
  if (!Firebase.setString(firebaseData, "/status/kipas", kipasStatus)) allSuccess = false;
  if (!Firebase.setString(firebaseData, "/status/exhaust", exhaustStatus)) allSuccess = false;
  if (!Firebase.setBool(firebaseData, "/status/door_open", doorOpen)) allSuccess = false;
  if (!Firebase.setBool(firebaseData, "/status/temp_protection", tempProtectionActive)) allSuccess = false;
  if (!Firebase.setBool(firebaseData, "/status/humidity_control", humidityControlActive)) allSuccess = false;
  
  // Send humidity control settings
  if (!Firebase.setFloat(firebaseData, "/settings/humidity_min", targetHumidityMin)) allSuccess = false;
  if (!Firebase.setFloat(firebaseData, "/settings/humidity_max", targetHumidityMax)) allSuccess = false;
  
  // Send session data if running
  if (systemRunning && !sessionId.isEmpty()) {
    String sessionPath = "/sessions/" + sessionId + "/data/" + String(millis());
    
    FirebaseJson json;
    json.set("timestamp", getTimestamp());
    json.set("temperature", suhuSensor);
    json.set("humidity", humiditySensor);
    json.set("moisture", kadarAir);
    json.set("weight", beratSekarang);
    json.set("status", processStatus);
    json.set("heater", pemanasStatus);
    json.set("fan", kipasStatus);
    json.set("exhaust", exhaustStatus);
    json.set("humidity_control", humidityControlActive);
    
    if (!Firebase.setJSON(firebaseData, sessionPath, json)) allSuccess = false;
  }
  
  // Update system heartbeat 
  Firebase.setString(firebaseData, "/system/last_update", getTimestamp());
  
  if (allSuccess) {
    Serial.println("🎉 All Firebase data sent successfully!");
    clearError("FIREBASE_FAILED");
  } else {
    addError("FIREBASE_FAILED", "Some Firebase operations failed");
    Serial.println("⚠  Some Firebase operations failed");
  }
}

// ======================================================================================================
void sendSessionStart() {
  if (WiFi.status() != WL_CONNECTED) return;
  
  String sessionPath = "/sessions/" + sessionId;
  
  FirebaseJson json;
  json.set("session_id", sessionId);
  json.set("start_time", getTimestamp());
  json.set("initial_weight", beratAwal);
  json.set("initial_humidity", humiditySensor);
  json.set("humidity_target_min", targetHumidityMin);
  json.set("humidity_target_max", targetHumidityMax);
  json.set("status", "active");
  
  Firebase.setJSON(firebaseData, sessionPath + "/info", json);
  Firebase.setString(firebaseData, "/system/current_session", sessionId);
  Firebase.setString(firebaseData, "/system/process_status", "running");
}

// ======================================================================================================
void sendSessionEnd() {
  if (WiFi.status() != WL_CONNECTED) return;
  
  String sessionPath = "/sessions/" + sessionId;
  
  FirebaseJson json;
  json.set("end_time", getTimestamp());
  json.set("final_weight", beratSekarang);
  json.set("final_moisture", kadarAir);
  json.set("final_humidity", humiditySensor);
  json.set("status", "completed");
  
  Firebase.updateNode(firebaseData, sessionPath + "/info", json);
  Firebase.setString(firebaseData, "/system/process_status", "completed");
}

// ======================================================================================================
void sendSystemStatus(String status) {
  if (WiFi.status() == WL_CONNECTED) {
    Firebase.setString(firebaseData, "/system/status", status);
    Firebase.setString(firebaseData, "/system/last_boot", getTimestamp());
    Firebase.setString(firebaseData, "/system/version", "v3.3_humidity_control");
  }
}

// Test DHT22 Sensor Functions ======================================================================================================
void testDHTSensor() {
  float testTemp = dht.readTemperature();
  float testHumid = dht.readHumidity();
  
  if (isnan(testTemp)) {
    addError("DHT22_TEMP_FAILED", "Temperature sensor check wiring");
    Serial.println("❌ DHT22 temperature sensor ERROR");
  } else {
    clearError("DHT22_TEMP_FAILED");
    Serial.println("✅ DHT22 temperature sensor OK - Temp: " + String(testTemp) + "°C");
  }
  
  if (isnan(testHumid)) {
    addError("DHT22_HUMID_FAILED", "Humidity sensor check wiring");
    Serial.println("❌ DHT22 humidity sensor ERROR");
  } else {
    clearError("DHT22_HUMID_FAILED");
    Serial.println("✅ DHT22 humidity sensor OK - RH: " + String(testHumid) + "%");
  }
}

// Test HX711 & Load Cell Sensor Functions ======================================================================================================
void testLoadCell() {
  if (scale.is_ready()) {
    scale.set_scale(465.284);
    Serial.println("✅ HX711 load cell initialized");
    clearError("SCALE_NOT_READY");
  } else {
    addError("SCALE_NOT_READY", "Load cell check wiring");
    Serial.println("❌ HX711 load cell ERROR");
  }
}

//Test All Output ======================================================================================================
void testAllOutputs() {
  Serial.println("   Testing Relay 1 (Heater Warm)...");
  digitalWrite(RELAY1_PIN, RELAY_ON);
  delay(1000);
  digitalWrite(RELAY1_PIN, RELAY_OFF);
  
  Serial.println("   Testing Relay 2 (Heater Hot)...");
  digitalWrite(RELAY2_PIN, RELAY_ON);
  delay(1000);
  digitalWrite(RELAY2_PIN, RELAY_OFF);
  
  Serial.println("   Testing Fan Motor 1 (Heater Panel)...");
  ledcWrite(fanPanChannel, 255);
  delay(1000);
  ledcWrite(fanColChannel, 0);

  Serial.println("   Testing Fan Motor 2 (Heater Colector)...");
  ledcWrite(fanColChannel, 255);
  delay(1000);
  ledcWrite(fanColChannel, 0);
  
  Serial.println("   Testing MOSFET 1 (Exhaust Fan) - HUMIDITY CONTROL...");
  digitalWrite(MOSFET1_PIN, HIGH);
  delay(1000);
  digitalWrite(MOSFET1_PIN, LOW);
  
  Serial.println("   Testing MOSFET 2 (Steam Pusher - Reserved)...");
  digitalWrite(MOSFET2_PIN, HIGH);
  delay(1000);
  digitalWrite(MOSFET2_PIN, LOW);
  
  Serial.println("   Testing Servo (Exhaust Valve) - HUMIDITY CONTROL...");
  exhaustServo.write(45);
  delay(1000);
  exhaustServo.write(90);
  delay(1000);
  exhaustServo.write(0);
  
  Serial.println("✅ All output tests completed");
}

// Fuzzy Logic Setup ======================================================================================================
void setupFuzzyLogic() {
  // Input 1: Suhu
  FuzzyInput *suhu = new FuzzyInput(1);
  suhu->addFuzzySet(dinginSuhu);
  suhu->addFuzzySet(normalSuhu);
  suhu->addFuzzySet(panasSuhu);
  fuzzy->addFuzzyInput(suhu);
  
  // Input 2: Kadar Air
  FuzzyInput *moisture = new FuzzyInput(2);
  moisture->addFuzzySet(keringKadar);
  moisture->addFuzzySet(normalKadar);
  moisture->addFuzzySet(lembabKadar);
  fuzzy->addFuzzyInput(moisture);
  
  // Input 3: Humidity
  FuzzyInput *humidity = new FuzzyInput(3);
  humidity->addFuzzySet(rendahHumidity);
  humidity->addFuzzySet(idealHumidity);
  humidity->addFuzzySet(tinggiHumidity);
  fuzzy->addFuzzyInput(humidity);
  
  // Output 1: Pemanas
  FuzzyOutput *pemanas = new FuzzyOutput(1);
  pemanas->addFuzzySet(matiPemanas);
  pemanas->addFuzzySet(hangatPemanas);
  pemanas->addFuzzySet(panasPemanas);
  fuzzy->addFuzzyOutput(pemanas);
  
  // Output 2: Kipas
  FuzzyOutput *kipas = new FuzzyOutput(2);
  kipas->addFuzzySet(matiKipas);
  kipas->addFuzzySet(lambatKipas);
  kipas->addFuzzySet(cepatKipas);
  fuzzy->addFuzzyOutput(kipas);
  
  // Output 3: Exhaust
  FuzzyOutput *exhaust = new FuzzyOutput(3);
  exhaust->addFuzzySet(tutupExhaust);
  exhaust->addFuzzySet(sedangExhaust);
  exhaust->addFuzzySet(bukaExhaust);
  fuzzy->addFuzzyOutput(exhaust);
}

// ======================================================================================================
void setupFuzzyRules() {
  // Rules untuk Heater dan Fan
  
  // R1: IF Dingin AND Kering THEN Mati AND Mati
  FuzzyRuleAntecedent *r1_antecedent = new FuzzyRuleAntecedent();
  r1_antecedent->joinWithAND(dinginSuhu, keringKadar);
  FuzzyRuleConsequent *r1_consequent = new FuzzyRuleConsequent();
  r1_consequent->addOutput(matiPemanas);
  r1_consequent->addOutput(matiKipas);
  FuzzyRule *rule1 = new FuzzyRule(1, r1_antecedent, r1_consequent);
  fuzzy->addFuzzyRule(rule1);
  
  // R2: IF Dingin AND Normal THEN Panas AND Cepat
  FuzzyRuleAntecedent *r2_antecedent = new FuzzyRuleAntecedent();
  r2_antecedent->joinWithAND(dinginSuhu, normalKadar);
  FuzzyRuleConsequent *r2_consequent = new FuzzyRuleConsequent();
  r2_consequent->addOutput(panasPemanas);
  r2_consequent->addOutput(cepatKipas);
  FuzzyRule *rule2 = new FuzzyRule(2, r2_antecedent, r2_consequent);
  fuzzy->addFuzzyRule(rule2);
  
  // R3: IF Dingin AND Lembab THEN Panas AND Cepat
  FuzzyRuleAntecedent *r3_antecedent = new FuzzyRuleAntecedent();
  r3_antecedent->joinWithAND(dinginSuhu, lembabKadar);
  FuzzyRuleConsequent *r3_consequent = new FuzzyRuleConsequent();
  r3_consequent->addOutput(panasPemanas);
  r3_consequent->addOutput(cepatKipas);
  FuzzyRule *rule3 = new FuzzyRule(3, r3_antecedent, r3_consequent);
  fuzzy->addFuzzyRule(rule3);
  
  // R4: IF Normal AND Kering THEN Mati AND Mati
  FuzzyRuleAntecedent *r4_antecedent = new FuzzyRuleAntecedent();
  r4_antecedent->joinWithAND(normalSuhu, keringKadar);
  FuzzyRuleConsequent *r4_consequent = new FuzzyRuleConsequent();
  r4_consequent->addOutput(matiPemanas);
  r4_consequent->addOutput(matiKipas);
  FuzzyRule *rule4 = new FuzzyRule(4, r4_antecedent, r4_consequent);
  fuzzy->addFuzzyRule(rule4);
  
  // R5: IF Normal AND Normal THEN Hangat AND Lambat
  FuzzyRuleAntecedent *r5_antecedent = new FuzzyRuleAntecedent();
  r5_antecedent->joinWithAND(normalSuhu, normalKadar);
  FuzzyRuleConsequent *r5_consequent = new FuzzyRuleConsequent();
  r5_consequent->addOutput(hangatPemanas);
  r5_consequent->addOutput(lambatKipas);
  FuzzyRule *rule5 = new FuzzyRule(5, r5_antecedent, r5_consequent);
  fuzzy->addFuzzyRule(rule5);
  
  // R6: IF Normal AND Lembab THEN Hangat AND Lambat
  FuzzyRuleAntecedent *r6_antecedent = new FuzzyRuleAntecedent();
  r6_antecedent->joinWithAND(normalSuhu, lembabKadar);
  FuzzyRuleConsequent *r6_consequent = new FuzzyRuleConsequent();
  r6_consequent->addOutput(hangatPemanas);
  r6_consequent->addOutput(lambatKipas);
  FuzzyRule *rule6 = new FuzzyRule(6, r6_antecedent, r6_consequent);
  fuzzy->addFuzzyRule(rule6);
  
  // R7: IF Panas AND Kering THEN Mati AND Mati
  FuzzyRuleAntecedent *r7_antecedent = new FuzzyRuleAntecedent();
  r7_antecedent->joinWithAND(panasSuhu, keringKadar);
  FuzzyRuleConsequent *r7_consequent = new FuzzyRuleConsequent();
  r7_consequent->addOutput(matiPemanas);
  r7_consequent->addOutput(matiKipas);
  FuzzyRule *rule7 = new FuzzyRule(7, r7_antecedent, r7_consequent);
  fuzzy->addFuzzyRule(rule7);
  
  // R8: IF Panas AND Normal THEN Mati AND Lambat
  FuzzyRuleAntecedent *r8_antecedent = new FuzzyRuleAntecedent();
  r8_antecedent->joinWithAND(panasSuhu, normalKadar);
  FuzzyRuleConsequent *r8_consequent = new FuzzyRuleConsequent();
  r8_consequent->addOutput(matiPemanas);
  r8_consequent->addOutput(lambatKipas);
  FuzzyRule *rule8 = new FuzzyRule(8, r8_antecedent, r8_consequent);
  fuzzy->addFuzzyRule(rule8);
  
  // R9: IF Panas AND Lembab THEN Mati AND Cepat
  FuzzyRuleAntecedent *r9_antecedent = new FuzzyRuleAntecedent();
  r9_antecedent->joinWithAND(panasSuhu, lembabKadar);
  FuzzyRuleConsequent *r9_consequent = new FuzzyRuleConsequent();
  r9_consequent->addOutput(matiPemanas);
  r9_consequent->addOutput(cepatKipas);
  FuzzyRule *rule9 = new FuzzyRule(9, r9_antecedent, r9_consequent);
  fuzzy->addFuzzyRule(rule9);
  
  Serial.println("✅ Fuzzy logic rules configured for Heater & Fan only");
  Serial.println("   Exhaust system controlled by humidity directly");
}

// Error Management Functions ======================================================================================================
void addError(String code, String message) {
  // Check if error already exists
  for (int i = 0; i < errorCount; i++) {
    if (systemErrors[i].code == code && systemErrors[i].active) {
      return; // Error already exists
    }
  }
  
  // Add new error
  if (errorCount < 10) {
    systemErrors[errorCount].code = code;
    systemErrors[errorCount].message = message;
    systemErrors[errorCount].active = true;
    systemErrors[errorCount].timestamp = millis();
    errorCount++;
    hasActiveErrors = true;
    
    Serial.println("❌ ERROR ADDED: [" + code + "] " + message);
    
    // Send error to Firebase
    if (WiFi.status() == WL_CONNECTED) {
      Firebase.setString(firebaseData, "/system/last_error", code + ": " + message);
      Firebase.setString(firebaseData, "/system/error_time", getTimestamp());
    }
  }
}

// ======================================================================================================
void clearError(String code) {
  for (int i = 0; i < errorCount; i++) {
    if (systemErrors[i].code == code && systemErrors[i].active) {
      systemErrors[i].active = false;
      Serial.println("✅ ERROR CLEARED: [" + code + "]");
      break;
    }
  }
  
  // Check if any errors still active
  hasActiveErrors = false;
  for (int i = 0; i < errorCount; i++) {
    if (systemErrors[i].active) {
      hasActiveErrors = true;
      break;
    }
  }
  
  // Clear Firebase error if no more active errors
  if (!hasActiveErrors && WiFi.status() == WL_CONNECTED) {
    Firebase.setString(firebaseData, "/system/last_error", "No active errors");
  }
}

// ======================================================================================================
void clearAllErrors() {
  for (int i = 0; i < errorCount; i++) {
    systemErrors[i].active = false;
  }
  hasActiveErrors = false;
  errorCount = 0;
  Serial.println("✅ All errors cleared");
  
  // Clear Firebase errors
  if (WiFi.status() == WL_CONNECTED) {
    Firebase.setString(firebaseData, "/system/last_error", "All errors cleared");
    Firebase.setString(firebaseData, "/system/error_time", getTimestamp());
  }
}