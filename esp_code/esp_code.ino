#include "config.h"
#include "globals.h"
#include "sensors/SensorManager.h"
#include "control/FuzzyController.h"
#include "control/ManualController.h"
#include "control/HumidityController.h"
#include "control/TemperatureController.h"
#include "display/DisplayManager.h"
#include "network/WiFiManager.h"
#include "network/FirebaseManager.h"
#include "network/WeatherAPI.h"
#include "storage/OfflineStorage.h"
#include "system/ErrorManager.h"
#include "system/ButtonHandler.h"
#include "system/StateManager.h"
#include "utils/TimeUtils.h"
#include "utils/Calculations.h"

// ============ HARDWARE INITIALIZATION ============
DHT dht(DHT_PIN, DHTTYPE);
HX711 scale;
LiquidCrystal_I2C lcd(0x27, 16, 2);
Servo exhaustServo;
FirebaseData firebaseData;
FirebaseConfig firebaseConfig;
FirebaseAuth firebaseAuth;
Fuzzy *fuzzy = new Fuzzy();

// ============ FUZZY SETS INITIALIZATION ============
FuzzySet *dinginSuhu = new FuzzySet(0, 0, 50, 60);
FuzzySet *normalSuhu = new FuzzySet(50, 60, 60, 70);
FuzzySet *panasSuhu = new FuzzySet(60, 70, 100, 100);
FuzzySet *rendahHumidity = new FuzzySet(0, 0, 25, 35);
FuzzySet *idealHumidity = new FuzzySet(25, 35, 35, 45);
FuzzySet *tinggiHumidity = new FuzzySet(35, 45, 100, 100);
FuzzySet *matiPemanas = new FuzzySet(0, 0, 85, 127);
FuzzySet *hangatPemanas = new FuzzySet(85, 127, 127, 170);
FuzzySet *panasPemanas = new FuzzySet(127, 170, 255, 255);
FuzzySet *matiKipas = new FuzzySet(0, 0, 85, 127);
FuzzySet *lambatKipas = new FuzzySet(85, 127, 127, 170);
FuzzySet *cepatKipas = new FuzzySet(127, 170, 255, 255);
FuzzySet *tutupExhaust = new FuzzySet(0, 0, 30, 50);
FuzzySet *sedangExhaust = new FuzzySet(30, 50, 70, 90);
FuzzySet *bukaExhaust = new FuzzySet(70, 90, 100, 100);
FuzzySet *keringKadar = new FuzzySet(0, 0, 11, 50);
FuzzySet *normalKadar = new FuzzySet(11, 50, 50, 91);
FuzzySet *lembabKadar = new FuzzySet(50, 91, 100, 100);

// ============ GLOBAL VARIABLES DEFINITION ============
int dst = DST;
unsigned long lastConfigCheck = 0;
unsigned long stableStartTime = 0;
int timezone = TIMEZONE;

const unsigned long weatherInterval = WEATHER_UPDATE_INTERVAL;
float tempOutside = 0.0;
String weatherDesc = "";
String weatherMain = "";
unsigned long lastWeatherUpdate = 0;

float targetHumidityMax = 40.0;
float targetHumidityMin = 30.0;
float targetTemperatureMax = 80.0;
float targetTemperatureMin = 60.0;
float targetMoisture = 12.0;

String processStatus = "Siap";
String sessionId = "";
bool stabilizing = false;
bool systemRunning = false;

bool humidityControlActive = false;
bool tempProtectionActive = false;
bool temperatureControlActive = false;

String exhaustStatus = "Tutup";
String kipasStatus = "Mati";
String pemanasStatus = "Mati";

float beratAwal = 0;
float beratSekarang = 0;
bool doorOpen = false;
float kadarAir = 0;
float kelembapanSensor = 0;
float lastWeight = 0;
float suhuSensor = 0;

bool manualExhaustControl = false;
bool manualExhaustState = false;
bool manualFanColControl = false;
bool manualFanColState = false;
bool manualFanPanControl = false;
bool manualFanPanState = false;
bool manualHeaterControl = false;
bool manualHeaterState = false;

SystemState currentState = WAIT_TARE;

SystemError systemErrors[10];
int errorCount = 0;
bool hasActiveErrors = false;

InfoDisplayMode currentInfoMode = INFO_DEFAULT;
bool infoDisplayActive = false;
unsigned long infoDisplayStart = 0;

std::vector offlineBuffer;
bool offlineMode = false;

ButtonState infoButton = {false, false, 0, 0, 0, false};
ButtonState startButton = {false, false, 0, 0, 0, false};
ButtonState tareButton = {false, false, 0, 0, 0, false};

// ============ SETUP ============
void setup() {
  Serial.begin(115200);
  Serial.println("\n🚀 SISTEM PENGERING ESP32 v3.3");
  Serial.println("===================================");
  
  // Initialize managers
  DisplayManager::init();
  SensorManager::init();
  ErrorManager::init();
  
  if (!SPIFFS.begin(true)) {
    Serial.println("❌ SPIFFS initialization failed!");
  }
  
  WeatherAPI::fetchWeather();
  FirebaseManager::loadConfig();
  
  // Initialize pins
  initializePins();
  
  // Test systems
  testAllSystems();
  
  // Network setup
  WiFiManager::connect();
  FirebaseManager::setup();
  
  // Time setup
  configTime(timezone, dst, "pool.ntp.org", "time.nist.gov");
  
  // Fuzzy setup
  FuzzyController::setup();
  
  DisplayManager::updateDefault();
  FirebaseManager::sendSystemStatus("ready");
  
  Serial.println("🚀 ENHANCED SYSTEM READY!");
  Serial.println("💡 LONG PRESS TARE BUTTON TO RESTART SYSTEM");
}

// ============ LOOP ============
void loop() {
  ButtonHandler::handleAll();
  ManualController::check();
  DisplayManager::updateDoorStatus();
  TemperatureController::checkProtection();
  
  if (millis() - lastWeatherUpdate > weatherInterval) {
    WeatherAPI::fetchWeather();
    lastWeatherUpdate = millis();
  }
  
  HumidityController::check();
  TemperatureController::checkControl();
  DisplayManager::handleInfoTimeout();
  
  StateManager::run();
  DisplayManager::update();
  
  if (WiFi.status() == WL_CONNECTED && offlineMode) {
    OfflineStorage::sync();
  }
  
  if (millis() - lastConfigCheck > 5000) {
    FirebaseManager::listenConfigUpdates();
    lastConfigCheck = millis();
  }
  
  delay(200);
}

// ============ HELPER FUNCTIONS ============
void initializePins() {
  ledcAttach(FANHEATPAN1, PWM_FREQ, PWM_RESOLUTION);
  ledcAttach(FANHEATCOL2, PWM_FREQ, PWM_RESOLUTION);
  pinMode(TARE_SWITCH, INPUT_PULLUP);
  pinMode(START_SWITCH, INPUT_PULLUP);
  pinMode(DOOR_SWITCH, INPUT_PULLUP);
  pinMode(INFO_SWITCH, INPUT_PULLUP);
  pinMode(RESET_SWITCH, INPUT_PULLUP);
  pinMode(RELAY1_PIN, OUTPUT);
  pinMode(RELAY2_PIN, OUTPUT);
  pinMode(MOSFET1_PIN, OUTPUT);
  pinMode(MOSFET2_PIN, OUTPUT);
  
  turnOffAllOutputs();
  
  exhaustServo.attach(SERVO_PIN);
  exhaustServo.write(0);
  
  Serial.println("✅ GPIO pins configured");
}

void testAllSystems() {
  Serial.println("🔧 Testing systems...");
  ButtonHandler::testPins();
  SensorManager::testDHT();
  SensorManager::testLoadCell();
  testOutputs();
}

void testOutputs() {
  Serial.println("   Testing outputs...");
  digitalWrite(RELAY1_PIN, RELAY_ON);
  delay(1000);
  digitalWrite(RELAY1_PIN, RELAY_OFF);
  
  digitalWrite(RELAY2_PIN, RELAY_ON);
  delay(1000);
  digitalWrite(RELAY2_PIN, RELAY_OFF);
  
  ledcWrite(FANHEATPAN1, 255);
  delay(1000);
  ledcWrite(FANHEATPAN1, 0);
  
  ledcWrite(FANHEATCOL2, 255);
  delay(1000);
  ledcWrite(FANHEATCOL2, 0);
  
  digitalWrite(MOSFET1_PIN, HIGH);
  delay(1000);
  digitalWrite(MOSFET1_PIN, LOW);
  
  exhaustServo.write(90);
  delay(1000);
  exhaustServo.write(0);
  
  Serial.println("✅ All output tests completed");
}

void turnOffAllOutputs() {
  digitalWrite(RELAY1_PIN, RELAY_OFF);
  digitalWrite(RELAY2_PIN, RELAY_OFF);
  digitalWrite(MOSFET1_PIN, LOW);
  digitalWrite(MOSFET2_PIN, LOW);
  ledcWrite(FANHEATPAN1, 0);
  ledcWrite(FANHEATCOL2, 0);
  exhaustServo.write(0);
  
  if (!tempProtectionActive) {
    pemanasStatus = "Mati";
    kipasStatus = "Mati";
    exhaustStatus = "Tutup";
  }
}