// LIBRARIES
// OLED 
#include <Adafruit_BusIO_Register.h>
#include <Adafruit_GenericDevice.h>
#include <Adafruit_I2CDevice.h>
#include <Adafruit_I2CRegister.h>
#include <Adafruit_SPIDevice.h>
#include <Adafruit_GFX.h>
#include <Adafruit_GrayOLED.h>
#include <Adafruit_SPITFT.h>
#include <Adafruit_SPITFT_Macros.h>
#include <gfxfont.h>
#include <Adafruit_SH110X.h>
#include <splash.h>

// Telegrambot
#include <UniversalTelegramBot.h>
#include <ArduinoJson.h>
#include <ArduinoJson.hpp>

// WIFI 
#include <WiFi.h>
#include <WiFiClientSecure.h>
#include "esp_wpa2.h" // WPA2-Enterprise support

//NeoPixel 
#include <Adafruit_NeoPixel.h>

//Sensor
#include <Ultrasonic.h> 
int distance = 150; // cm

//Scheduling 
#include <time.h>
///////////////////////////////

#define USE_ENTERPRISE_WIFI
//#define USE_HOME_WIFI

// Telegram credentials & security
const char* BOT_TOKEN = "8362864589:AAE2wdQavaOMRrrIMdnhRNUgU6fjNEI2SNc";
WiFiClientSecure secured_client;
UniversalTelegramBot bot(BOT_TOKEN, secured_client);

// Tasks (items) state
bool taskActive = false;
String currentTask = "";
bool softwareOff = false;

//Checklist storage 
const int MAX_TASKS = 10; 
String taskList[MAX_TASKS];
int taskCount = 0; 

//Scheduling globals
bool timeReady = false; //becames true after NTP sync

// functions in another tab 
//void connectHomeWifi(); //only for tests at home
void connectEnterpriseWiFi();
void checkTelegram();
void initButton();
void checkButton();
void initDisplay();
void updateDisplay();
void handleNewMessages (int numNewMessages);
void neoBegin();
void neoModeA();
void neoModeB();
void neoModeC();
void neoOff();
void addTask (String t);
void removeTaskAt (int index);
void syncCurrentTask();
void setupTime();
void syncCurrentTask();
void addTaskImmediate(String t);
bool handleMaybeScheduledCommand(String text, String chat_id);
void removeTaskAt(int index);
void initPowerSwitch();
bool handlePowerSwitch();
bool deviceIsOn();

String lastDrawnTask = "__INIT__";
bool lastDrawnActive = false;

//button Mode C state
bool whiteActive = false; 
unsigned long whiteStartTime = 0; 
const int BUTTON_PIN = D7;
const int BUTTON2_PIN = D4;

//power switch
const int POWER_SWITCH_PIN = D12;

//TelegramTimer
unsigned long telegramTimer = millis();

void setup() {
  Serial.begin(9600);
  initPowerSwitch();
  initButton();
  
  neoBegin();
  neoModeA(); // start warm lamp

  
  initDisplay();

  connectEnterpriseWiFi();
  //connectHomeWifi();
  Serial.println("\nWiFi connected");

  setupTime();

  // HTTPS for Telegram in prototype
  secured_client.setInsecure();
  Serial.println("Telegram ready!");
}

void handleNewMessages(int numNewMessages) {
  for (int i = 0; i < numNewMessages; i++) {
    String text = bot.messages[i].text;
    String chat_id = bot.messages[i].chat_id;

    // ignore Telegram commands if you want
    if (text == "/start") continue;

    
    if (!handleMaybeScheduledCommand(text, chat_id)) {
      addTaskImmediate(text);
    }
    if (taskActive) neoModeB(); // keep this as you had it
    // currentTask = text;
    // taskActive = true;
    // neoModeB(); // purple ON immediately !!!!!!!

    Serial.println("Task received: " + currentTask);
  }
}

void loop() {
  if (!handlePowerSwitch()) return;
  checkButton();

  if (softwareOff) {
    neoOff();
    return; // ignore everything until physical switch cycle
  } 

  syncCurrentTask();

  if(!(whiteActive && taskActive) && millis() - telegramTimer > 5000){
    checkTelegram();
    telegramTimer = millis();
    Serial.println("Telegram");
  }
  

  // Mode C 
  if (whiteActive && taskActive) {
    if (digitalRead(BUTTON_PIN) == 1 || digitalRead(BUTTON2_PIN) == 1) {
      neoModeC(); // white flash
    } else {
      if (taskCount > 0) removeTaskAt(taskCount - 1);
      whiteActive = false; 
      if (taskActive) neoModeB();
      else neoModeA();

      updateDisplay();
      return;

    }
  }

  // Mode A/B
  else if (!taskActive) {
    ultrasonicOff(); 
    neoModeA(); 
    whiteActive = false;
  }
  else {
    readDistance();
    neoModeB(); //purple and blue waves
  }

  updateDisplay();

}
