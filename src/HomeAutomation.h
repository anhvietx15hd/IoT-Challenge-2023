#ifndef _HOME_AUTOMATION_
#define _HOME_AUTOMATION_
#include <Arduino.h>
#include <Adafruit_INA219.h>
#include <PubSubClient.h>
#include <ArduinoJson.h>
#include "menu.h"
#include "config.h"
#include "ConnectionConfig.h"
#include <ESP32_Servo.h> 


/************************************************
 * APIs
*************************************************/
extern DynamicJsonDocument doc; 
extern Adafruit_INA219 ina219;
extern Servo myservo;
extern WiFiClient espClient;
extern PubSubClient client;

/*The last time a message was published the server*/
extern long lastSentMsg;
extern long lastTimeBuzzer;
extern int16_t timeToUpdate;
extern int16_t timeTurnOnLock;

extern float power;
extern bool lightSensor;
extern bool humanDetected;
/*Device status*/
extern bool ceilingLightStatus; //Normal Close

extern bool yardLightSwitchStatus;
extern bool hallwayLightSwitchStatus;
extern bool warning_Security;
extern bool hallwayLightStatus;
extern bool yardLightStatus;
extern bool Flag_wait;
extern bool state_Door;
extern String Human;
extern bool state_Lw;
extern bool FlagRecognize;
extern bool FlagOnDoor;
extern bool WarningState;
extern bool FlagWarning;
extern bool checkMail;
extern portMUX_TYPE mux;



/**
 * @brief This function to read sensors data
 * 
 * @param pvParameters To pass the warning when no parameter
 */
void ReadSensors(void *pvParameters);

/**
 * @brief This function to Security 
 * 
 * @param pvParameters To pass the warning when no parameter
 */


void callback(char* topic, byte* payload, uint16_t length);

/**
 * @brief This function to control electric device 
 * 
 */
void controlDevice(void);
void doorOpen(void);
void doorClose(void);
void recognize_check(void);
void lock_mode(void);
void auto_on_lock_door(void);
void Security(void);
void auto_open_lock(void);
#endif /* Home Automation*/
/**********************************************************
 * End of file
***********************************************************/