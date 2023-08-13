#ifndef _CONNECTION_CONFIG_
#define _CONNECTION_CONFIG_
#include <Arduino.h>
#include <WiFi.h>
#include <WebServer.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>
#include <PubSubClient.h>
#include <EEPROM.h>
#include "menu.h"
/************************************************
 * Prototype
*************************************************/
/*WIFI*/
extern String wifi_ssid;
extern String wifi_password;
/*MQTT*/
extern String mqtt_server;
extern String mqtt_username;
extern String mqtt_password;
extern String mqtt_port;
extern String publish_topic;
extern String subscribe_topic;

extern WebServer server;
extern uint16_t statusCode;

extern WiFiClient espClient;
extern PubSubClient client;

extern bool isConfigButtonPressed;
extern long time_warning;


/**
 * @brief This function to set up wifi connection for client
 * 
 */
void setupWifi(void);

/**
 * @brief This function to set up MQTT connection to server
 * 
 */
void setupMQTTConnection(void);

/**
 * @brief This function to set up a web server to config connection
 * and save to EEPROM
 * 
 */
void getConfig(void);

/**
 * @brief This fucntion to write config data to EEPROM
 * 
 * @param[in] index Data to write 
 * @param[in] add Address in EEPROM
 */
void writeEEPROM(String index, uint16_t add);

/**
 * @brief This function to read data from EEPROM for setting up connection
 * 
 */
void readEEPROM(void);

/**
 * @brief Create a web server for connection configure
 * 
 */
void createWebServer(void);

/**
 * @brief Launch the web for user connection using wifi
 * 
 */
void launchWeb(void);

/**
 * @brief Set up Wifi AP so that user can scan wifi by their device
 * 
 */
void setupAP(void);
#endif /*Connection Config*/
/******************************************************
 * End of file
*******************************************************/