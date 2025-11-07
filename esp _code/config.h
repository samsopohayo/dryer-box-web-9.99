#ifndef CONFIG_H
#define CONFIG_H

#include 
#include 
#include 
#include 
#include 
#include 
#include 
#include 
#include 
#include 
#include 

// ============ FIREBASE CONFIGURATION ============
#define FIREBASE_HOST "dryer-box-default-rtdb.asia-southeast1.firebasedatabase.app"
#define FIREBASE_AUTH "AIzaSyCkXIB8zbRrR27K2kmt_S39pCKLQT5Wzls"
#define WIFI_SSID "Hehe"
#define WIFI_PASSWORD "1234567899"

// ============ PIN DEFINITIONS ============
#define DHT_PIN 4           // Sensor DHT22
#define DOUT_PIN 14         // Sensor loadcell
#define SCK_PIN 12          // Sensor loadcell
#define TARE_SWITCH 32      // TARE button (LONG PRESS untuk restart)
#define START_SWITCH 33     // START button
#define INFO_SWITCH 35      // INFO button 
#define RESET_SWITCH 34     // RESET button 
#define DOOR_SWITCH 23      // Door switch
#define FANHEATPAN1 26      // Kipas heater panel 1
#define FANHEATCOL2 25      // Kipas heater colector 2
#define MOSFET1_PIN 16      // Kipas exhaust
#define MOSFET2_PIN 15      // Steam pusher (reserved)
#define RELAY1_PIN 17       // Heater Hangat
#define RELAY2_PIN 5        // Heater Panas
#define SERVO_PIN 18        // Exhaust valve
#define SDA_PIN 21          // I2C LCD SDA
#define SCL_PIN 22          // I2C LCD SCL

// ============ SYSTEM CONFIGURATION ============
#define DHTTYPE DHT22
#define RELAY_ON  LOW
#define RELAY_OFF HIGH
#define MAX_TEMP_LIMIT 80.0
#define INFO_DISPLAY_TIMEOUT 10000
#define HUMIDITY_HYSTERESIS 2.0
#define TEMPERATURE_HYSTERESIS 2.0

// ============ PWM CONFIGURATION ============
#define PWM_FREQ 20000
#define PWM_RESOLUTION 8

// ============ WEATHER API ============
#define WEATHER_UPDATE_INTERVAL 1800000  // 30 minutes

// ============ TIME CONFIGURATION ============
#define TIMEZONE 7 * 3600
#define DST 0

#endif