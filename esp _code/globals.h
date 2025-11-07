#ifndef GLOBALS_H
#define GLOBALS_H

#include "config.h"
#include 

// ============ HARDWARE OBJECTS ============
extern DHT dht;
extern HX711 scale;
extern LiquidCrystal_I2C lcd;
extern Servo exhaustServo;
extern FirebaseData firebaseData;
extern FirebaseConfig firebaseConfig;
extern FirebaseAuth firebaseAuth;
extern Fuzzy *fuzzy;

// ============ FUZZY SETS ============
// Input Fuzzy Sets - Suhu
extern FuzzySet *dinginSuhu;
extern FuzzySet *normalSuhu;
extern FuzzySet *panasSuhu;

// Input Fuzzy Sets - Humidity
extern FuzzySet *rendahHumidity;
extern FuzzySet *idealHumidity;
extern FuzzySet *tinggiHumidity;

// Output Fuzzy Sets - Pemanas
extern FuzzySet *matiPemanas;
extern FuzzySet *hangatPemanas;
extern FuzzySet *panasPemanas;

// Output Fuzzy Sets - Kipas
extern FuzzySet *matiKipas;
extern FuzzySet *lambatKipas;
extern FuzzySet *cepatKipas;

// Output Fuzzy Sets - Exhaust 
extern FuzzySet *tutupExhaust;
extern FuzzySet *sedangExhaust;
extern FuzzySet *bukaExhaust;

// Input Fuzzy Sets - Kadar Air
extern FuzzySet *keringKadar;
extern FuzzySet *normalKadar;
extern FuzzySet *lembabKadar;

// ============ ENUMS ============
enum SystemState {
  WAIT_TARE,
  WAIT_OBJECT,
  WAIT_START,
  RUNNING
};

enum InfoDisplayMode {
  INFO_DEFAULT,
  INFO_SENSORS,
  INFO_WEIGHTS,
  INFO_WIFI,
  INFO_HUMADITY,
  INFO_TEMP,
  INFO_HISTORY
};

// ============ STRUCTS ============
struct SystemError {
  String code;
  String message;
  bool active;
  unsigned long timestamp;
};

struct ButtonState {
  bool currentPressed;
  bool lastPressed;
  unsigned long lastPressTime;
  unsigned long lastReleaseTime;
  int clickCount;
  bool waitingForDoubleClick;
};

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

// ============ GLOBAL VARIABLES - Time ============
extern int dst;
extern unsigned long lastConfigCheck;
extern unsigned long stableStartTime;
extern int timezone;

// ============ GLOBAL VARIABLES - Weather ============
extern const unsigned long weatherInterval;
extern float tempOutside;
extern String weatherDesc;
extern String weatherMain;
extern unsigned long lastWeatherUpdate;

// ============ GLOBAL VARIABLES - Target Parameters ============
extern float targetHumidityMax;
extern float targetHumidityMin;
extern float targetTemperatureMax;
extern float targetTemperatureMin;
extern float targetMoisture;

// ============ GLOBAL VARIABLES - Process Status ============
extern String processStatus;
extern String sessionId;
extern bool stabilizing;
extern bool systemRunning;

// ============ GLOBAL VARIABLES - Control Status ============
extern bool humidityControlActive;
extern bool tempProtectionActive;
extern bool temperatureControlActive;

// ============ GLOBAL VARIABLES - Actuator Status ============
extern String exhaustStatus;
extern String kipasStatus;
extern String pemanasStatus;

// ============ GLOBAL VARIABLES - Sensor Data ============
extern float beratAwal;
extern float beratSekarang;
extern bool doorOpen;
extern float kadarAir;
extern float kelembapanSensor;
extern float lastWeight;
extern float suhuSensor;

// ============ GLOBAL VARIABLES - Manual Control ============
extern bool manualExhaustControl;
extern bool manualExhaustState;
extern bool manualFanColControl;
extern bool manualFanColState;
extern bool manualFanPanControl;
extern bool manualFanPanState;
extern bool manualHeaterControl;
extern bool manualHeaterState;

// ============ GLOBAL VARIABLES - System States ============
extern SystemState currentState;

// ============ GLOBAL VARIABLES - Error System ============
extern SystemError systemErrors[10];
extern int errorCount;
extern bool hasActiveErrors;

// ============ GLOBAL VARIABLES - Info Display ============
extern InfoDisplayMode currentInfoMode;
extern bool infoDisplayActive;
extern unsigned long infoDisplayStart;

// ============ GLOBAL VARIABLES - Offline Storage ============
extern std::vector offlineBuffer;
extern bool offlineMode;

// ============ GLOBAL VARIABLES - Button States ============
extern ButtonState infoButton;
extern ButtonState startButton;
extern ButtonState tareButton;

#endif 