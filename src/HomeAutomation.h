#ifndef _HOME_AUTOMATION_
#define _HOME_AUTOMATION_
#include <Arduino.h>
#include <Adafruit_INA219.h>
#include <DHT.h>
#include <PubSubClient.h>
#include <ArduinoJson.h>
#include "menu.h"
#include "config.h"
#include "ConnectionConfig.h"
/************************************************
 * APIs
*************************************************/
extern DynamicJsonDocument doc; 
extern Adafruit_INA219 ina219;
extern DHT dht;
extern WiFiClient espClient;
extern PubSubClient client;

/*The last time a message was published the server*/
extern long lastSentMsg;
extern int16_t timeToUpdate;

extern float power;
extern bool lightSensor;
extern uint8_t gas;
extern bool humanDetected;
extern float humidity;
extern float temperature;
/*Device status*/
extern bool ceilingLightStatus; //Normal Close
extern bool wallLightStatus; //Normal Close
extern bool security;

/**
 * @brief This function to read sensors data
 * 
 * @param pvParameters To pass the warning when no parameter
 */
void ReadSensors(void *pvParameters);

/**
 * @brief This function to send and receive data to/from server
 * 
 * @param pvParameters 
 */
void DataProcessing(void *pvParameters);

/**
 * @brief This function to process data which received from server
 * 
 * @param topic 
 * @param payload 
 * @param length 
 */
void callback(char* topic, byte* payload, uint16_t length);

/**
 * @brief This function to control electric device 
 * 
 */
void controlDevice(void);
#endif /* Home Automation*/
/**********************************************************
 * End of file
***********************************************************/